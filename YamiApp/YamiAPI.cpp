#include "YamiAPI.h"

using namespace YAMI;

#pragma region PARAMETERS

bool YAMIConfig::EnableDebug = false;
EPlatform YAMIConfig::CurrentPlatform = EPlatform::PLATF_WINDOWS;

bool Utility::isInitialized = false;

OnInfo Utility::onLog;

std::vector<Utility::ALLOC_INFO> Utility::g_LeakList;
std::mutex Utility::g_LeakMutex;

std::clock_t Utility::initTime;
std::clock_t Utility::currentTime;

float Utility::deltaTime = 0.0f;

FILE* Utility::pConsole = nullptr;

const int Utility::MAX_KEY_LENGTH = 256;

bool Utility::isRecordedMouseMovement = false;

std::vector<std::pair<INPUT, INPUT>> Utility::recordedInputs;

//long Utility::initialTime = 0;
std::chrono::time_point<std::chrono::steady_clock> Utility::initialTime;

#pragma endregion

#pragma region OBJECT

#if !defined(YAMI_MACOS)
void Object::operator delete(void* ptr)
{
	if (Object* v = reinterpret_cast<Object*>(ptr)) {
		v->~Object();
	}
	::operator delete(ptr);
	ptr = nullptr;
}
#endif

#pragma endregion

#pragma region STRUCT FUNCTIONS

bool quaternion_t::operator==(const quaternion_t& _o) const
{
	return MATH_EQUALS(x, _o.x) && MATH_EQUALS(y, _o.y) && MATH_EQUALS(z, _o.z) && MATH_EQUALS(w, _o.w);
}

bool quaternion_t::operator!=(const quaternion_t& _o) const
{
	return !(*this == _o);
}

quaternion_t quaternion_t::operator+(const quaternion_t& _o) const
{
	return quaternion_t(x + _o.x, y + _o.y, z + _o.z, w + _o.w);
}

quaternion_t quaternion_t::operator*(float _v) const
{
	return quaternion_t(_v * x, _v * y, _v * z, _v * w);
}

quaternion_t quaternion_t::operator*(const quaternion_t& _o) const
{
	quaternion_t temp;
	temp.w = (w * _o.w) - (x * _o.x) - (y * _o.y) - (z * _o.z);
	temp.x = (w * _o.x) + (x * _o.w) + (y * _o.z) - (z * _o.y);
	temp.y = (w * _o.y) - (x * _o.z) + (y * _o.w) + (z * _o.x);
	temp.z = (w * _o.z) + (x * _o.y) - (y * _o.x) + (z * _o.w);

	return temp;
}

quaternion_t& quaternion_t::operator*=(float _v)
{
	x *= _v;
	y *= _v;
	z *= _v;
	w *= _v;
	return *this;
}

quaternion_t& quaternion_t::operator*=(const quaternion_t& _o)
{
	return (*this = _o * (*this));
}

quaternion_t quaternion_t::dotProduct(const quaternion_t& _o) const
{
	return (x * _o.x) + (y * _o.y) + (z * _o.z) + (w * _o.w);
}

quaternion_t quaternion_t::identity()
{
	quaternion_t tmp;
	tmp.w = 1.f;
	tmp.x = 0.f;
	tmp.y = 0.f;
	tmp.z = 0.f;
	return tmp;
}

quaternion_t& quaternion_t::normalize()
{
	float n = x * x + y * y + z * z + w * w;
	if (MATH_EQUALS(n, 1.0f)) {
		return *this;
	}
	else if (MATH_EQUALS(n, 0.0f)) {
		DEBUG_WARNING("Quaternion with invalid values");
		return *this;
	}
	n = 1.0f / std::sqrt(n);
	return (*this *= n);
}

quaternion_t& quaternion_t::inverse()
{
	x = -x; y = -y; z = -z;
	return *this;
}

quaternion_t quaternion_t::fromAngleAxis(float _angle, const vector3d_t& _axis)
{
	float fHalfAngle = 0.5f * _angle;
	float fSin = sinf(fHalfAngle);
	quaternion_t res;
	res.w = cosf(fHalfAngle);
	res.x = fSin * _axis.x;
	res.y = fSin * _axis.y;
	res.z = fSin * _axis.z;
	return res;
}

void quaternion_t::toAngleAxis(float& _angle, vector3d_t& _axis) const
{
	float scale = sqrtf(x * x + y * y + z * z);

	if (MATH_EQUALS(scale, 0.0f) || w > 1.0f || w < -1.0f)
	{
		_angle = 0.0f;
		_axis.x = 0.0f;
		_axis.y = 1.0f;
		_axis.z = 0.0f;
	}
	else
	{
		float invscale = 1.f / scale;
		_angle = 2.0f * acosf(w);
		_axis.x = x * invscale;
		_axis.y = y * invscale;
		_axis.z = z * invscale;
	}
}

vector3d_t quaternion_t::toEuler() const
{
	bool allowNegative = false;
	vector3d_t euler;

	// if the input quaternion is normalized, this is exactly one. Otherwise, this acts as a correction factor for the quaternion's not-normalizedness
	float unit = (x * x) + (y * y) + (z * z) + (w * w);

	// this will have a magnitude of 0.5 or greater if and only if this is a singularity case
	float test = x * w - y * z;

	if (test > 0.4995f * unit) // singularity at north pole
	{
		euler.x = M_PI / 2;
		euler.y = 2.0f * atan2(y, x);
		euler.z = 0;
	}
	else if (test < -0.4995f * unit) // singularity at south pole
	{
		euler.x = -M_PI / 2;
		euler.y = -2.0f * atan2(y, x);
		euler.z = 0;
	}
	else // no singularity - this is the majority of cases
	{
		euler.x = asin(2.0f * (w * x - y * z));
		euler.y = atan2(2.0f * w * y + 2.0f * z * x, 1 - 2.0f * (x * x + y * y));
		euler.z = atan2(2.0f * w * z + 2.0f * x * y, 1 - 2.0f * (z * z + x * x));
	}

	// all the math so far has been done in radians. Before returning, we convert to degrees...
	euler = MATH_RAD2DEG(euler);

	//...and then ensure the degree values are between 0 and 360
	euler.x = fmod(euler.x, 360.0f);
	euler.y = fmod(euler.y, 360.0f);
	euler.z = fmod(euler.z, 360.0f);

	if (!allowNegative)
	{
		if (euler.x < 0) euler.x = 360 + euler.x;
		if (euler.y < 0) euler.y = 360 + euler.y;
		if (euler.z < 0) euler.z = 360 + euler.z;
	}

	return euler;
}

quaternion_t quaternion_t::fromEuler(const vector3d_t& _euler)
{
	float xOver2 = MATH_RAD2DEG(_euler.x) * 0.5f;
	float yOver2 = MATH_RAD2DEG(_euler.y) * 0.5f;
	float zOver2 = MATH_RAD2DEG(_euler.z) * 0.5f;

	float sinXOver2 = sin(xOver2);
	float cosXOver2 = cos(xOver2);
	float sinYOver2 = sin(yOver2);
	float cosYOver2 = cos(yOver2);
	float sinZOver2 = sin(zOver2);
	float cosZOver2 = cos(zOver2);

	quaternion_t result;
	result.x = cosYOver2 * sinXOver2 * cosZOver2 + sinYOver2 * cosXOver2 * sinZOver2;
	result.y = sinYOver2 * cosXOver2 * cosZOver2 - cosYOver2 * sinXOver2 * sinZOver2;
	result.z = cosYOver2 * cosXOver2 * sinZOver2 - sinYOver2 * sinXOver2 * cosZOver2;
	result.w = cosYOver2 * cosXOver2 * cosZOver2 + sinYOver2 * sinXOver2 * sinZOver2;

	return result;
}

quaternion_t quaternion_t::fromRotationVector(const vector3d_t& _from, const vector3d_t& _to)
{
	// Based on Stan Melax's article in Game Programming Gems
	// Copy, since cannot modify local
	vector3d_t v0 = _from;
	vector3d_t v1 = _to;
	v0.normalize();
	v1.normalize();
	float d = v0.dotProduct(v1);
	if (d >= 1.0f) // If dot == 1, vectors are the same
	{
		return identity();
	}
	else if (d <= -1.0f) // exactly opposite
	{
		vector3d_t axis(1.0f, 0.f, 0.f);
		axis = axis.crossProduct(v0);
		if (axis.getSize() == 0)
		{
			axis = vector3d_t(0.f, 1.f, 0.f);
			axis = axis.crossProduct(v0);
		}
		// same as fromAngleAxis(core::PI, axis).normalize();
		return quaternion_t(axis.x, axis.y, axis.z, 0).normalize();
	}
	float s = std::sqrt((1 + d) * 2); // optimize inv_sqrt
	float invs = 1.f / s;
	vector3d_t c = v0.crossProduct(v1) * invs;
	return quaternion_t(c.x, c.y, c.z, s * 0.5f).normalize();
}

quaternion_t& quaternion_t::operator=(const quaternion_t& _o)
{
	x = _o.x;
	y = _o.y;
	z = _o.z;
	w = _o.w;
	return *this;
}

vector3d_t& vector3d_t::operator=(const vector3d_t& _o)
{
	x = _o.x;
	y = _o.y;
	z = _o.z;
	return *this;
}

vector3d_t vector3d_t::operator-() const
{
	return vector3d_t(-x, -y, -z);
}

vector3d_t vector3d_t::operator+(const vector3d_t& other) const
{
	return vector3d_t(y + other.x, y + other.y, z + other.z);
}

vector3d_t& vector3d_t::operator+=(const vector3d_t& other)
{
	x += other.x; y += other.y; z += other.z; return *this;
}

vector3d_t vector3d_t::operator+(const float val) const
{
	return vector3d_t(x + val, y + val, z + val);
}

vector3d_t& vector3d_t::operator+=(const float val)
{
	x += val; y += val; z += val; return *this;
}

vector3d_t vector3d_t::operator-(const vector3d_t& other) const
{
	return vector3d_t(x - other.x, y - other.y, z - other.z);
}

vector3d_t& vector3d_t::operator-=(const vector3d_t& other)
{
	x -= other.x; y -= other.y; z -= other.z; return *this;
}

vector3d_t vector3d_t::operator-(const float val) const
{
	return vector3d_t(x - val, y - val, z - val);
}

vector3d_t& vector3d_t::operator-=(const float val)
{
	x -= val; y -= val; z -= val; return *this;
}

vector3d_t vector3d_t::operator*(const vector3d_t& other) const
{
	return vector3d_t(x * other.x, y * other.y, z * other.z);
}

vector3d_t& vector3d_t::operator*=(const vector3d_t& other)
{
	x *= other.x; y *= other.y; z *= other.z; return *this;
}

vector3d_t vector3d_t::operator*(const float v) const
{
	return vector3d_t(x * v, y * v, z * v);
}

vector3d_t& vector3d_t::operator*=(const float v)
{
	x *= v; y *= v; z *= v; return *this;
}

vector3d_t vector3d_t::operator/(const vector3d_t& other) const
{
	return vector3d_t(x / other.x, y / other.y, z / other.z);
}

vector3d_t& vector3d_t::operator/=(const vector3d_t& other)
{
	x /= other.x; y /= other.y; z /= other.z; return *this;
}

vector3d_t vector3d_t::operator/(const float v) const
{
	float i = (float)1.0 / v; return vector3d_t(x * i, y * i, z * i);
}

vector3d_t& vector3d_t::operator/=(const float v)
{
	float i = (float)1.0 / v; x *= i; y *= i; z *= i; return *this;
}

bool vector3d_t::operator==(const vector3d_t& _o) const
{
	return MATH_EQUALS(x, _o.x) && MATH_EQUALS(y, _o.y) && MATH_EQUALS(z, _o.z);
}

bool vector3d_t::operator!=(const vector3d_t& _o) const
{
	return !(*this == _o);
}

float vector3d_t::getSize() const
{
	return std::sqrt(x * x + y * y + z * z);
}

float vector3d_t::getSizeSquared() const
{
	return x * x + y * y + z * z;
}

vector3d_t& vector3d_t::normalize()
{
	float length = getSizeSquared();
	if (MATH_EQUALS(length, 0.0f)) // this check isn't an optimization but prevents getting NAN in the sqrt.
		return *this;
	length = 1.0f / std::sqrt(length);

	x = x * length;
	y = y * length;
	z = z * length;
	return *this;
}

float vector3d_t::dotProduct(const vector3d_t& _o) const
{
	return x * _o.x + y * _o.y + z * _o.z;
}

vector3d_t vector3d_t::crossProduct(const vector3d_t& _o) const
{
	return vector3d_t(y * _o.z - z * _o.y, z * _o.x - x * _o.z, x * _o.y - y * _o.x);
}

#pragma endregion

#pragma region MEMORY FUNCTIONS

void* Utility::AddTrack(void* p, unsigned int asize, const char* fname, unsigned int lnum)
{
	ALLOC_INFO info;

	info.address = p;
	strncpy_s(info.file, fname, _MAX_PATH - 1);
	info.line = lnum;
	info.size = asize;
	info.readyToDelete = false;
	//g_LeakMutex.lock();
	g_LeakList.push_back(info);
	//g_LeakMutex.unlock();

	return p;
}

void Utility::ForceRemoveTrack(void* p)
{
	for (unsigned int i = 0; i < g_LeakList.size(); i++) {
		if (g_LeakList[i].address == p)
		{
			std::lock_guard<std::mutex> UtilityLock(YAMI::Utility::g_LeakMutex);
			//g_LeakMutex.lock();
			if (IS_VALID(g_LeakList[i].address)) {
				if (Object* obj = reinterpret_cast<Object*>(g_LeakList[i].address)) {
					delete obj;
				}
				else {
					delete g_LeakList[i].address;
				}
				g_LeakList[i].address = NULL;
			}
			g_LeakList.erase(g_LeakList.begin() + i);
			//g_LeakMutex.unlock();
			break;
		}
	}
}

void Utility::ForceRemoveTrackArray(void* p)
{
	for (unsigned int i = 0; i < g_LeakList.size(); i++) {
		if (g_LeakList[i].address == p)
		{
			std::lock_guard<std::mutex> UtilityLock(YAMI::Utility::g_LeakMutex);
			//g_LeakMutex.lock();
			if (IS_VALID(g_LeakList[i].address)) {
				delete[] g_LeakList[i].address;
				g_LeakList[i].address = NULL;
			}
			g_LeakList.erase(g_LeakList.begin() + i);
			//g_LeakMutex.unlock();
			break;
		}
	}
}

void Utility::RemoveTrack(void* p)
{
	//g_LeakMutex.lock();
	for (ALLOC_INFO& info : g_LeakList) {
		if (info.address == p)
		{
			info.readyToDelete = true;
			break;
		}
	}
	//g_LeakMutex.unlock();
}

bool Utility::GarbageCollector()
{
	std::lock_guard<std::mutex> UtilityLock(YAMI::Utility::g_LeakMutex);
	bool res = false;
	//g_LeakMutex.lock();
	for (unsigned int i = 0; i < g_LeakList.size(); i++) {
		if (!IS_VALID(g_LeakList[i].address)) {
			g_LeakList.erase(g_LeakList.begin() + i);
			i--;
		}
		else if (g_LeakList[i].readyToDelete)
		{
			res = true;
			if (Object* obj = reinterpret_cast<Object*>(g_LeakList[i].address)) {
				delete obj;
			}
			else {
				delete g_LeakList[i].address;
			}
			g_LeakList[i].address = NULL;
			g_LeakList.erase(g_LeakList.begin() + i);
			i--;
		}
	}
	return res;
	//g_LeakMutex.unlock();
}

void Utility::FreeAllMemory()
{
	std::lock_guard<std::mutex> UtilityLock(YAMI::Utility::g_LeakMutex);
	//g_LeakMutex.lock();
	unsigned long totalSize = 0;

	for (unsigned int i = 0; i < g_LeakList.size(); i++) {
		DEBUG_INFO(g_LeakList[i].file, g_LeakList[i].line, ": ADDRESS ", (INT_PTR)g_LeakList[i].address, "\t", g_LeakList[i].size, " unfreed\n");
		totalSize += g_LeakList[i].size;
		delete g_LeakList[i].address;
	}
	g_LeakList.clear();

	//g_LeakMutex.unlock();

	DEBUG_NONE("-----------------------------------------------------------\n");
	DEBUG_INFO("Total Unfreed: ", totalSize, " bytes\n");
}

#pragma endregion

#pragma region STRING FUNCTIONS

std::string Utility::ToString(std::initializer_list<std::string> strList)
{
	std::string ret = "";
	for (auto s = strList.begin(); s != strList.end(); s++) {
		if (IS_VALID(s)) {
			ret += *s;
		}
	}
	return ret;
}

std::string Utility::ToString(std::string val) {
	return val;
}

std::string Utility::ToString(unsigned char val) {
	return std::to_string(static_cast<int>(val));
}

std::string hexmap[] = { "0", "1", "2", "3", "4", "5", "6", "7",
					   "8", "9", "a", "b", "c", "d", "e", "f" };

std::string Utility::ToStringHex(unsigned char val) {
	return hexmap[(val & 0xF0) >> 4] + hexmap[(val & 0x0F)];
}

std::string Utility::ToString(const char* val) {
	if (IS_VALID(val)) {
		return std::string(val);
	}
	else {
		return "";
	}
}

std::string Utility::ToString(const wchar_t* val) {
	if (IS_VALID(val)) {
		std::wstring res = std::wstring(val);
		return std::string(res.begin(), res.end());
	}
	else {
		return "";
	}
}

std::string Utility::ToString(std::wstring val) {
	return std::string(val.begin(), val.end());
}

std::wstring Utility::ToWString(const wchar_t* val) {
	return std::wstring(val);
}

std::wstring Utility::ToWString(std::string val) {
	return std::wstring(val.begin(), val.end());
}

bool Utility::endsWith(std::string const& fullString, std::string const& ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}

#pragma endregion

#pragma region TIME FUNCTIONS

void Utility::CalculateDelta() {
	currentTime = clock();
	deltaTime = float(currentTime - initTime) / CLOCKS_PER_SEC;
	initTime = currentTime;
}

float Utility::GetDeltaTime() {
	return deltaTime;
}

int Utility::GetFPS() {
	float delta = GetDeltaTime();
	if (delta > 0.0f) {
		return static_cast<int>(CLOCKS_PER_SEC / delta);
	}
	return static_cast<int>(CLOCKS_PER_SEC);
}

#pragma endregion

#pragma region LOG FUNCTIONS

void Utility::InitDebug() {
	if (ENABLE_DEBUG_LOG) {
#if defined(YAMI_WINDOWS)
		if (pConsole == NULL) {
			AllocConsole();
			freopen_s(&pConsole, "CONOUT$", "wb", stdout);
		}
#elif defined(YAMI_LINUX) || defined(YAMI_MACOS)
		const char* logFile = "/tmp/YAMI.log";
		char  pidarg[256]; // the '--pid=' argument of tail
		pid_t child;       // the pid of the child proc
		pid_t parent;      // the pid of the parent proc
		int   fn;          // file no of fp

		// Open file for redirection
		pConsole = fopen(logFile, "w");
		fn = fileno(pConsole);

		// Get pid of current process and create string with argument for tail
		parent = getpid();
		sprintf(pidarg, "--pid=%d", parent);

		// Create child process
		child = fork();
		if (child == 0) {
			// CHILD PROCESS

			// Replace child process with a gnome-terminal running:
			//      tail -f /tmp/asdf.log --pid=<parent_pid>
			// This prints the lines outputed in asdf.log and exits when
			// the parent process dies.
			//execlp("gnome-terminal", "gnome-terminal", "-x", "tail", "-f", logFile, pidarg, NULL);
			execlp("gnome-terminal", "gnome-terminal", "--", "tail", "-f", logFile, pidarg, NULL);

			// if there's an error, print out the message and exit
			perror("execlp()");
			//exit(-1);
			return;
		}
		else {
			// PARENT PROCESS
			close(1);      // close stdout
			int ok = dup2(fn, 1); // replace stdout with the file

			if (ok != 1) {
				perror("dup2()");
				return; //-1
			}

			// Make stdout flush on newline, doesn't happen by default
			// since stdout is actually a file at this point.
			setvbuf(stdout, NULL, _IONBF, BUFSIZ);
		}
#endif
	}
}

void Utility::FinishDebug() {
	if (ENABLE_DEBUG_LOG){ //&& COMPLETE_DELETE) {
#if defined(YAMI_WINDOWS) || defined(YAMI_LINUX) || defined(YAMI_MACOS)
		if (pConsole != NULL) {
			fclose(pConsole);
		}
		pConsole = NULL;
#endif
	}
}

void Utility::OnLog(std::string text, EDebug type) {
	if (onLog.isValid()) {
		onLog(text, type);
	}
}

#pragma endregion

#pragma region DATA FUNCTIONS

int Utility::ConcatData(uint8_t* data, unsigned int size) {
	if (size <= 1) {
		return (char)CONCAT_BYTES_1(data[0]);
	}
	else if (size == 2) {
		return (short)CONCAT_BYTES_2(data[0], data[1]);
	}
	else if (size == 3) {
		return (int)CONCAT_BYTES_3(data[0], data[1], data[2]);
	}
	else if (size == 4) {
		return (int)CONCAT_BYTES_4(data[0], data[1], data[2], data[3]);
	}
	else {
		return 0;
	}
}

#pragma endregion

#pragma region EXCEPTION AND VALID FUNCTIONS

#ifdef YAMI_WINDOWS

void Utility::excp_transl_func(unsigned int u, PEXCEPTION_POINTERS)
{
	throw YAMI_Exception(u);
}

#endif

bool Utility::IsValid(INT_PTR ptr)
{
	return ptr != 0x00000000 && ptr != 0xfeeefeee && ptr != 0xdddddddd && ptr != 0xcdcdcdcd
		&& ptr != 0xbdbdbdbd && ptr != 0xabababab && ptr != 0xdeadbeef && ptr != 0xbaadf00d
		&& ptr != 0xcccccccc && ptr != 0xfdfdfdfd && ptr != 0xcdcdcdcdcdcdcdcd
		&& ptr != 0xfffffffd; //for Android
}

#pragma endregion

#pragma region RECORD KEYS

#ifdef YAMI_WINDOWS //Only for Windows (for now)

bool Utility::IsKeyPressed(WORD key) {
	return GetAsyncKeyState(key) & 0x8000;
}

bool Utility::IsKeyReleased(WORD key) {
	return !(GetAsyncKeyState(key) & 0x8000);
}

std::vector<std::pair<INPUT, INPUT>> Utility::RecordKeyboardInput(bool recordMouseMovement, WORD endKey) {
	DEBUG_INFO("Iniciando grabación de teclado. Presiona 'End' para detener.");
	recordedInputs.clear();
	//initialTime = GetTickCount64();
	initialTime = std::chrono::steady_clock::now();
	isRecordedMouseMovement = recordMouseMovement;
	bool shiftPressed = false;
	bool controlPressed = false;
	bool altPressed = false;
	bool tabPressed = false;
	bool leftClickPressed = false;
	bool rightClickPressed = false;
	while (true) {
		INPUT mouseInput;
		mouseInput.type = INPUT_MOUSE;
		if (isRecordedMouseMovement) {
			POINT cursorPos;
			GetCursorPos(&cursorPos);
			//mouseInput.mi.time = GetTickCount64() - initialTime;
			std::chrono::duration<double> elapsedTime = std::chrono::steady_clock::now() - initialTime;
			mouseInput.mi.time = elapsedTime.count() * 1000;
			mouseInput.mi.dx = cursorPos.x;
			mouseInput.mi.dy = cursorPos.y;
		}

		for (int i = 0; i < MAX_KEY_LENGTH; i++) {
			if (GetAsyncKeyState(i) == -32767) {
				if (i == endKey) {
					DEBUG_INFO("Grabación detenida.");
					return recordedInputs;
				}
				else if (i == VK_SHIFT) shiftPressed = true;
				else if (i == VK_CONTROL) controlPressed = true;
				else if (i == VK_MENU) altPressed = true;
				else if (i == VK_TAB) tabPressed = true;
				INPUT input;
				input.type = INPUT_KEYBOARD;
				input.ki.wScan = 0;
				//input.ki.time = GetTickCount64() - initialTime;
				std::chrono::duration<double> elapsedTime = std::chrono::steady_clock::now() - initialTime;
				input.ki.time = elapsedTime.count() * 1000;
				input.ki.dwExtraInfo = 0;
				input.ki.wVk = i;
				input.ki.dwFlags = 0;
				if (i == VK_LBUTTON && isRecordedMouseMovement) {
					recordedInputs.push_back(std::make_pair(mouseInput, input));
					leftClickPressed = true;
				}
				else if(i == VK_RBUTTON && isRecordedMouseMovement){
					recordedInputs.push_back(std::make_pair(mouseInput, input));
					rightClickPressed = true;
				}
				else {
					recordedInputs.push_back(std::make_pair(mouseInput, input));
				}
			}
			else if (GetAsyncKeyState(i) == 0) {
				//if ((i == VK_SHIFT && shiftPressed) || (i == VK_CONTROL && controlPressed) && (i == VK_MENU && altPressed) && (i == VK_TAB && tabPressed)) {
				if ((i == VK_SHIFT && shiftPressed) || (i == VK_CONTROL && controlPressed) || (i == VK_MENU && altPressed) || (i == VK_TAB && tabPressed)) {
					if (i == VK_SHIFT && shiftPressed) shiftPressed = false;
					if (i == VK_CONTROL && controlPressed) controlPressed = false;
					if (i == VK_MENU && altPressed) altPressed = false;
					if (i == VK_TAB && tabPressed) tabPressed = false;
					INPUT input;
					input.type = INPUT_KEYBOARD;
					input.ki.wScan = 0;
					//input.ki.time = GetTickCount64() - initialTime;
					std::chrono::duration<double> elapsedTime = std::chrono::steady_clock::now() - initialTime;
					input.ki.time = elapsedTime.count() * 1000;
					input.ki.dwExtraInfo = 0;
					input.ki.wVk = i;
					input.ki.dwFlags = KEYEVENTF_KEYUP;
					recordedInputs.push_back(std::make_pair(mouseInput, input));
				}
				else if (((i == VK_LBUTTON && leftClickPressed) || (i == VK_RBUTTON && rightClickPressed)) && isRecordedMouseMovement) {
					if (i == VK_LBUTTON && leftClickPressed) leftClickPressed = false;
					if (i == VK_RBUTTON && rightClickPressed) rightClickPressed = false;
					INPUT input;
					input.type = INPUT_KEYBOARD;
					input.ki.wScan = 0;
					//input.ki.time = GetTickCount64() - initialTime;
					std::chrono::duration<double> elapsedTime = std::chrono::steady_clock::now() - initialTime;
					input.ki.time = elapsedTime.count() * 1000;
					input.ki.dwExtraInfo = 0;
					input.ki.wVk = i;
					input.ki.dwFlags = KEYEVENTF_KEYUP;
					recordedInputs.push_back(std::make_pair(mouseInput, input));
				}
			}
			else if (isRecordedMouseMovement) {
				recordedInputs.push_back(std::make_pair(mouseInput, mouseInput));
			}
		}
	}
	return recordedInputs;
}

void Utility::PlaybackKeyboardInput(const std::vector<std::pair<INPUT, INPUT>> data, long long timesToRepeat, long long timeToUpKeyMS, WORD endKey) {
	do {
		DEBUG_INFO("Reproduciendo grabación de teclado.");
		std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();
		for (auto& inputPair : (data.empty()? recordedInputs : data)) {
			auto mouse = inputPair.first;
			auto input = inputPair.second;
			for (int i = 0; i < MAX_KEY_LENGTH; i++) {
				if (GetAsyncKeyState(i) == -32767) {
					if (i == endKey) {
						DEBUG_INFO("Reproducción terminada.");
						return;
					}
				}
			}
			if (isRecordedMouseMovement) {
				POINT cursorPos;
				GetCursorPos(&cursorPos);
				if (mouse.mi.dx != cursorPos.x || mouse.mi.dy != cursorPos.y) {
					SetCursorPos(mouse.mi.dx, mouse.mi.dy);
				}
			}
			if (input.type == INPUT_KEYBOARD) {
				switch (input.ki.wVk) {
				case VK_LBUTTON:
					if (input.ki.dwFlags == 0 && isRecordedMouseMovement) {
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
						DEBUG_SPAM("MOUSEEVENTF_LEFTDOWN");
					}
					else if (input.ki.dwFlags == KEYEVENTF_KEYUP && isRecordedMouseMovement) {
						mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
						DEBUG_SPAM("MOUSEEVENTF_LEFTUP");
					}
					break;
				case VK_RBUTTON:
					if (input.ki.dwFlags == 0 && isRecordedMouseMovement) {
						mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
						DEBUG_SPAM("MOUSEEVENTF_RIGHTDOWN");
					}
					else if (input.ki.dwFlags == KEYEVENTF_KEYUP && isRecordedMouseMovement) {
						mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
						DEBUG_SPAM("MOUSEEVENTF_RIGHTUP");
					}
					break;
				default:
					std::chrono::duration<double> elapsedTime = std::chrono::steady_clock::now() - startTime;
					std::chrono::milliseconds sleepTime((int)(input.ki.time - elapsedTime.count() * 1000));
					std::this_thread::sleep_for(sleepTime);
					SendInput(1, &input, sizeof(INPUT));
					if (!(input.ki.dwFlags & KEYEVENTF_KEYUP) && (timeToUpKeyMS > 0 || input.ki.wVk == VK_CAPITAL || input.ki.wVk == VK_NUMLOCK)) {
						if (timeToUpKeyMS > 0) std::this_thread::sleep_for(std::chrono::milliseconds(timeToUpKeyMS));
						INPUT upKey;
						upKey.type = INPUT_KEYBOARD;
						upKey.ki.wScan = 0;
						upKey.ki.dwExtraInfo = 0;
						upKey.ki.time = input.ki.time;
						upKey.ki.wVk = input.ki.wVk;
						upKey.ki.dwFlags = KEYEVENTF_KEYUP;
						SendInput(1, &upKey, sizeof(INPUT));
					}
					break;
				}
			}
			else if (input.type == INPUT_MOUSE && isRecordedMouseMovement) {
				std::chrono::duration<double> elapsedTime = std::chrono::steady_clock::now() - startTime;
				std::chrono::milliseconds sleepTime((int)(input.mi.time - elapsedTime.count() * 1000));
				std::this_thread::sleep_for(sleepTime);
			}
		}
		if (timesToRepeat > 0) timesToRepeat--;
	} while (timesToRepeat < 0 || timesToRepeat > 0);
}

#endif

#pragma endregion