#pragma once
#include "stdafx.h"

namespace YAMI {

#pragma region DEFINITIONS

	/* Cpp Utils */
#define IS_VALID(val) YAMI::Utility::IsValid((INT_PTR)val)
#define GET_VIRTUAL_PTR(val) YAMI::Utility::GetVPtr(val)
#define NaN std::numeric_limits<float>::infinity()
#define EPSILON std::numeric_limits<float>::epsilon()
#define NAMEOF(...) #__VA_ARGS__
#define TO_WSTRING(s) YAMI::Utility::ToWString(s)
#define TO_STRING(s) YAMI::Utility::ToString(s)
#define TO_CONST_CHAR(s) s.c_str()
#define FIND(cont, val) std::find(cont.begin(), cont.end(), val)
#define CONTAINS(cont, val) (std::find(cont.begin(), cont.end(), val) != cont.end())
#define STR_EQUALS(str1, str2) (str1.compare(str2) == 0)
#define STR_FIND(str1, str2) (str1.find(str2) != string::npos)
#define STR_EMPTY(str) STR_EQUALS(str, "")

/* Maths Utils*/
#define MATH_MIN(val, minVal)  ((val <= minVal) ? val : minVal)
#define MATH_MAX(val, maxVal)  ((val >= maxVal) ? val : maxVal)
#define MATH_CLAMP(val, minVal, maxVal) ((val >= minVal) ? ((val <= maxVal) ? val : maxVal) : minVal)
#define MATH_NEARLY_EQUALS(a,b, tolerance) (fabs(a - b) <= tolerance * std::max(fabs(a), fabs(b)))
#define MATH_EQUALS(a,b) MATH_NEARLY_EQUALS(a,b,EPSILON)
#define MATH_RAD2DEG(radian) (radian * (M_PI / 180.0f) )
#define MAX_VALID_SIZE 10000000

/* Internal Utils*/
#define CONCAT_BYTES(v, size) YAMI::Utility::ConcatData(v, size)
#define CONCAT_BYTES_1(a) a
#define CONCAT_BYTES_2(a,b) (b << 8) | CONCAT_BYTES_1(a)
#define CONCAT_BYTES_3(a,b,c) (c << 16) | CONCAT_BYTES_2(a,b)
#define CONCAT_BYTES_4(a,b,c,d) (d << 24) | CONCAT_BYTES_3(a,b,c)
#define SAFE_CREATE std::lock_guard<std::mutex> UtilityLock(YAMI::Utility::g_LeakMutex);
#if defined(YAMI_WINDOWS)
#	define SAFE_EXCEPTION YAMI::Scoped_YAMI_Translator scoped_se_translator{ YAMI::Utility::excp_transl_func };
#else
#	define SAFE_EXCEPTION
#endif

/* Time Management */
#define INIT_TIME YAMI::Utility::CalculateDelta();
#define DELTA_TIME YAMI::Utility::GetDeltaTime()
#define FPS YAMI::Utility::GetFPS()
#define SLEEP(t) std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(t*1000)));

/* Debug Management */
#define ON_LOG(txt, type) {YAMI::Utility::OnLog(txt, type);}
#define ENABLE_DEBUG_LOG YAMI::YAMIConfig::EnableDebug
#define INIT_DEBUG YAMI::Utility::InitDebug();
#define FINISH_DEBUG YAMI::Utility::FinishDebug();
#define SEP_FOR_DEBUG ","
#define DEBUG_INFO(...) {YAMI::Utility::DebugInfo(__VA_ARGS__);}
#define DEBUG_WARNING(...) {YAMI::Utility::DebugWarning(__VA_ARGS__, " (ocurred in: ", __FILE__, ":", __LINE__, ")");}
#define DEBUG_ERROR(...) {YAMI::Utility::DebugError(__VA_ARGS__, " (ocurred in: ", __FILE__, ":", __LINE__, ")");}
#define DEBUG_EXCEPTION(...) {YAMI::Utility::DebugException(__VA_ARGS__, " (ocurred in: ", __FILE__, ":", __LINE__, ")");}
#define DEBUG_SPAM(...) {YAMI::Utility::DebugSpam(__VA_ARGS__);}
#define DEBUG_NONE(...) {YAMI::Utility::DebugNone(__VA_ARGS__);}
#define CURRENT_PLATFORM YAMI::YAMIConfig::CurrentPlatform


/* Memory Management */
#define NEW(clase, params)			reinterpret_cast<clase *>(YAMI::Utility::AddTrack(new clase params, sizeof(clase), __FILE__, __LINE__));
#define DEL(p)						{ YAMI::Utility::RemoveTrack(p); };
#define DEL_THIS					{ YAMI::Utility::RemoveTrack(this); };
#define FORCE_DEL(p)				{ YAMI::Utility::ForceRemoveTrack(p); delete p; p = NULL; };
#define FORCE_DEL_THIS				{ YAMI::Utility::ForceRemoveTrack(this); delete this;};
#define NEW_ARRAY(clase, num)		reinterpret_cast<clase *>(YAMI::Utility::AddTrack(new clase[num], sizeof(clase) * num, __FILE__, __LINE__));
#define DEL_ARRAY(p)				{ YAMI::Utility::ForceRemoveTrackArray(p); delete []p; p = NULL;};

#define GARBAGE_COLLECTOR YAMI::Utility::GarbageCollector()
#define FREE_MEMORY {YAMI::Utility::FreeAllMemory();}

/* Delegates */
#define CreateDelegate(obj, member, id) YAMI::Utility::_CreateDelegate(obj, member, id)
#define OnInfo YAMI::Utility::_OnInfo

/* Main Functions */
#define INIT_YAMI(_enableDebug, _logDelegate) {YAMI::Utility::Init(_enableDebug,_logDelegate);}
#define IS_INIT_YAMI YAMI::Utility::isInitialized()
#define UPDATE_YAMI {YAMI::Utility::Update();}
#define END_YAMI {YAMI::Utility::End();}

#pragma endregion

#pragma region ENUMS

	/// <summary>Level of importance of debug messages</summary>
	enum EDebug {
		/// <summary>Importance of info</summary>
		D_INFO = 0,
		/// <summary>Importance of warning</summary>
		D_WARNING = 1,
		/// <summary>Importance of error</summary>
		D_ERROR = 2,
		/// <summary>Importance of exception</summary>
		D_EXCEPTION = 3,
		/// <summary>Importance of spam</summary>
		D_SPAM = 4
	};

	enum EPlatform
	{
		PLATF_NONE = 0,
		PLATF_WINDOWS = 1,
		PLATF_ANDROID = 2,
		PLATF_LINUX = 3,
		PLATF_MACOS = 4,
		PLATF_IOS = 5
	};

#pragma endregion

#pragma region STRUCTS

	class YAMI_API YAMIConfig {
	public:
		static bool EnableDebug;
		static EPlatform CurrentPlatform;
	};

	extern "C" {

		/// <summary>To store the acceleration values from the IMU of the device</summary>
		struct vector3d_t {
			/// <summary> X component of the vector3d structure</summary>
			float x = 0.0f;
			/// <summary> Y component of the vector3d structure</summary>
			float y = 0.0f;
			/// <summary> Z component of the vector3d structure</summary>
			float z = 0.0f;
			vector3d_t(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
			{
				x = _x;
				y = _y;
				z = _z;
			};
			vector3d_t(const vector3d_t& _o)
			{
				x = _o.x;
				y = _o.y;
				z = _o.z;
			};

			YAMI_API vector3d_t& operator=(const vector3d_t& _o);

			YAMI_API vector3d_t operator-() const;

			YAMI_API vector3d_t operator+(const vector3d_t& _o) const;

			YAMI_API vector3d_t& operator+=(const vector3d_t& _o);

			YAMI_API vector3d_t operator+(const float _v) const;

			YAMI_API vector3d_t& operator+=(const float _v);

			YAMI_API vector3d_t operator-(const vector3d_t& _o) const;

			YAMI_API vector3d_t& operator-=(const vector3d_t& _o);

			YAMI_API vector3d_t operator-(const float _v) const;

			YAMI_API vector3d_t& operator-=(const float _v);

			YAMI_API vector3d_t operator*(const vector3d_t& _o) const;

			YAMI_API vector3d_t& operator*=(const vector3d_t& _o);

			YAMI_API vector3d_t operator*(const float _v) const;

			YAMI_API vector3d_t& operator*=(const float _v);

			YAMI_API vector3d_t operator/(const vector3d_t& _o) const;

			YAMI_API vector3d_t& operator/=(const vector3d_t& _o);

			YAMI_API vector3d_t operator/(const float _v) const;

			YAMI_API vector3d_t& operator/=(const float _v);

			YAMI_API bool operator==(const vector3d_t& _o) const;

			YAMI_API bool operator!=(const vector3d_t& _o) const;

			YAMI_API float getSize() const;

			YAMI_API float getSizeSquared() const;

			YAMI_API vector3d_t& normalize();

			YAMI_API float dotProduct(const vector3d_t& _o) const;

			YAMI_API vector3d_t crossProduct(const vector3d_t& _o) const;
		};

		/// <summary>To store the rotation values from the IMU of the device</summary>
		struct quaternion_t {
			/// <summary>
			/// X component of the quaternion structure
			/// </summary>
			float x = 0.0f;
			/// <summary>
			/// Y component of the quaternion structure
			/// </summary>
			float y = 0.0f;
			/// <summary>
			/// Z component of the quaternion structure
			/// </summary>
			float z = 0.0f;
			/// <summary>
			/// W component of the quaternion structure
			/// </summary>
			float w = 1.0f;

			quaternion_t(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 1.0f)
			{
				x = _x;
				y = _y;
				z = _z;
				w = _w;
			};

			quaternion_t(const quaternion_t& _o)
			{
				x = _o.x;
				y = _o.y;
				z = _o.z;
				w = _o.w;
			};

			YAMI_API quaternion_t& operator=(const quaternion_t& _o);

			YAMI_API bool operator==(const quaternion_t& _o) const;

			YAMI_API bool operator!=(const quaternion_t& _o) const;

			YAMI_API quaternion_t operator+(const quaternion_t& _o) const;

			YAMI_API quaternion_t operator*(float _v) const;

			YAMI_API quaternion_t operator*(const quaternion_t& _o) const;

			YAMI_API quaternion_t& operator*=(float _v);

			YAMI_API quaternion_t& operator*=(const quaternion_t& _o);

			YAMI_API quaternion_t dotProduct(const quaternion_t& _o) const;

			YAMI_API quaternion_t& normalize();

			YAMI_API quaternion_t& inverse();

			YAMI_API static quaternion_t identity();

			YAMI_API void toAngleAxis(float& _angle, vector3d_t& _axis) const;

			YAMI_API vector3d_t toEuler() const;

			YAMI_API static quaternion_t fromAngleAxis(float _angle, const vector3d_t& _axis);

			YAMI_API static quaternion_t fromEuler(const vector3d_t& _euler);

			YAMI_API static quaternion_t fromRotationVector(const vector3d_t& _from, const vector3d_t& _to);
		};

	};

#pragma endregion

#pragma region EXCEPTION CLASSES

#ifdef YAMI_WINDOWS
	class YAMI_Exception : public std::exception
	{
	private:
		const unsigned int nSE;
	public:
		YAMI_Exception() noexcept : YAMI_Exception{ 0 } {}
		YAMI_Exception(unsigned int n) noexcept : nSE{ n } {}
		unsigned int getSeNumber() const noexcept { return nSE; }
	};

	class Scoped_YAMI_Translator
	{
	private:
		const _se_translator_function old_SE_translator;
	public:
		Scoped_YAMI_Translator(_se_translator_function new_SE_translator) noexcept
			: old_SE_translator{ _set_se_translator(new_SE_translator) } {}
		~Scoped_YAMI_Translator() noexcept { _set_se_translator(old_SE_translator); }
	};
#endif

#pragma endregion

#pragma region TIMER

	class Timer {
		bool clear = true;
		bool pause = false;
		float time = 0.0f;
		bool debugMode = false;
		std::function<void()> onInit = {};
		std::function<void()> onDispose = {};
		std::function<void()> onPause = {}; //while being paused
	public:
		Timer() {

		};

		virtual ~Timer() {
			Dispose();
		};

		bool IsAlive() {
			return !this->clear;
		};

		bool IsPaused() {
			return this->pause;
		};

		template<typename Function>
		void SetTimeout(Function function, float delay) {
			this->clear = false;
			this->pause = false;
			this->time = delay;
			this->debugMode = ENABLE_DEBUG_LOG;
			std::thread t([=]() {
				std::thread::id this_id = std::this_thread::get_id(); //captured by value
				if (this->debugMode) {
					//std::cout << "Timeout - Thread id: " << this_id << std::endl;
				};
				if (onInit) onInit();
				if (this->clear) {
					if (onDispose) onDispose();
					return;
				}
				SLEEP(this->time);
				while (this->pause) {
					if (onPause) onPause();
					SLEEP(this->time);
				};
				if (this->clear) {
					if (onDispose) onDispose();
					return;
				}
				function();
				if (this->clear) {
					if (onDispose) onDispose();
					return;
				}
				});
			t.detach();
		};

		template<typename Function>
		void SetInterval(Function function, float interval) {
			Dispose();
			this->clear = false;
			this->pause = false;
			this->time = interval;
			this->debugMode = ENABLE_DEBUG_LOG;
			std::thread t([=]() {
				std::thread::id this_id = std::this_thread::get_id(); //captured by value
				if (onInit) onInit();
				while (true) {
					if (this->debugMode) {
						//std::cout << "Interval - Thread id: " << this_id << std::endl;
					};
					if (this->clear) {
						if (onDispose) onDispose();
						return;
					}
					if (this->pause) {
						if (onPause) onPause();
						continue;
					}
					function();
					SLEEP(this->time);
				}
				});
			t.detach();
		};

		template<typename Function>
		void SetOnInit(Function function) {
			onInit = [=]() {
				function();
			};
		}

		void Pause() {
			this->clear = false;
			this->pause = true;
		};

		template<typename Function>
		void SetOnPause(Function function) {
			onPause = [=]() {
				function();
			};
		}

		void Dispose() {
			this->pause = false;
			this->clear = true;
		};

		template<typename Function>
		void SetOnDispose(Function function) {
			onDispose = [=]() {
				function();
			};
		}
	};

#pragma endregion

	class Utility {
	public:
#pragma region DELEGATES

		template <typename RC, typename Class, typename... Args>
		class create_delegate {
		public:
			Class* d_object;
			RC(Class::* d_member)(Args...);
			INT_PTR d_id;

			create_delegate(Class* object, RC(Class::* member)(Args...), INT_PTR id)
				: d_object(object)
				, d_member(member)
				, d_id(id) {
			};
			RC operator()(Args... args) {
				return (this->d_object->*this->d_member)(std::forward<Args>(args)...);
			};
			bool operator== (create_delegate const& other) const {
				return this->d_object == other.d_object
					&& this->d_member == other.d_member;
			};
			bool operator!= (create_delegate const& other) const {
				return !(*this == other);
			};
		};

		template <typename RC, typename Class, typename... Args>
		static create_delegate<RC, Class, Args...> _CreateDelegate(Class& object, RC(Class::* member)(Args...), INT_PTR id) {
			return create_delegate<RC, Class, Args...>(&object, member, id);
		};

		template <typename Signature>
		struct delegate;

		template <typename... Args>
		struct delegate<void(Args...)>
		{
		public:
			struct base {
				virtual ~base() {}
				virtual bool do_cmp(base* other) = 0;
				virtual void do_call(Args... args) = 0;
				virtual bool is_same(INT_PTR id) = 0;
			};
			template <typename T>
			struct call : base {
				INT_PTR d_id;
				T d_callback;
				template <typename S>
				call(S&& callback) : d_id(0), d_callback(std::forward<S>(callback)) {}
				template<typename RC, typename Class, typename... Args2>
				call(create_delegate<RC, Class, Args2...>&& callback) : d_id((INT_PTR)callback.d_id), d_callback(std::forward<create_delegate<RC, Class, Args2...> >(callback)) {}

				bool do_cmp(base* other) {
					call<T>* tmp = dynamic_cast<call<T>*>(other);
					return tmp && this->d_callback == tmp->d_callback;
				}
				void do_call(Args... args) {
					return this->d_callback(std::forward<Args>(args)...);
				}

				bool is_same(INT_PTR id) {
					return this->d_id == id;
				}
			};
			std::vector<std::unique_ptr<base> > d_callbacks;
			delegate() {};
			delegate(delegate& val) {
				//this->d_callbacks.swap(val.d_callbacks);
				for (auto& el : val.d_callbacks) {
					auto it = std::find_if(this->d_callbacks.begin(),
						this->d_callbacks.end(),
						[&](std::unique_ptr<base>& other) {
							return el->do_cmp(other.get());
						});
					if (it == this->d_callbacks.end()) {
						this->d_callbacks.push_back(std::move(el));
					};
				};
			};
			template <typename T>
			delegate(T&& callback) {
				call<T> tmp(std::forward<T>(callback));
				auto it = std::find_if(this->d_callbacks.begin(),
					this->d_callbacks.end(),
					[&](std::unique_ptr<base>& other) {
						return tmp.do_cmp(other.get());
					});
				if (it == this->d_callbacks.end()) {
					this->d_callbacks.emplace_back(new call<T>(std::forward<T>(callback)));
				};
			};

#if defined(YAMI_ANDROID)
			bool operator==(delegate& val) {
				for (auto& el : val.d_callbacks) {
					bool found = false;
					for (auto& other : this->d_callbacks) {
						if (el->do_cmp(other.get())) {
							found = true;
							break;
						};
					};
					if (!found) {
						return false;
					};
				};
				return true;
			};
#endif

			void operator=(delegate& val) {
				//this->d_callbacks.swap(val.d_callbacks);
				for (auto& el : val.d_callbacks) {
					auto it = std::find_if(this->d_callbacks.begin(),
						this->d_callbacks.end(),
						[&](std::unique_ptr<base>& other) {
							return el->do_cmp(other.get());
						});
					if (it == this->d_callbacks.end()) {
						this->d_callbacks.push_back(std::move(el));
					};
				};
			};
			delegate& operator+= (delegate& val) {
				for (auto& el : val.d_callbacks) {
					auto it = std::find_if(this->d_callbacks.begin(),
						this->d_callbacks.end(),
						[&](std::unique_ptr<base>& other) {
							return el->do_cmp(other.get());
						});
					if (it == this->d_callbacks.end()) {
						this->d_callbacks.push_back(std::move(el));
					};
				};
				return *this;
			};
			delegate& operator-= (delegate& val) {
				for (auto& el : val.d_callbacks) {
					auto it = std::remove_if(this->d_callbacks.begin(),
						this->d_callbacks.end(),
						[&](std::unique_ptr<base>& other) {
							return el->do_cmp(other.get());
						});
					this->d_callbacks.erase(it, this->d_callbacks.end());
				};
				return *this;
			};
			template <typename T>
			delegate& operator+= (T&& callback) {
				call<T> tmp(std::forward<T>(callback));
				auto it = std::find_if(this->d_callbacks.begin(),
					this->d_callbacks.end(),
					[&](std::unique_ptr<base>& other) {
						return tmp.do_cmp(other.get());
					});
				if (it == this->d_callbacks.end()) {
					this->d_callbacks.emplace_back(new call<T>(std::forward<T>(callback)));
				};
				return *this;
			};
			template <typename T>
			delegate& operator-= (T&& callback) {
				call<T> tmp(std::forward<T>(callback));
				auto it = std::remove_if(this->d_callbacks.begin(),
					this->d_callbacks.end(),
					[&](std::unique_ptr<base>& other) {
						return tmp.do_cmp(other.get());
					});
				this->d_callbacks.erase(it, this->d_callbacks.end());
				return *this;
			};
			void operator()(Args... args) {
				for (int i = 0; i < this->d_callbacks.size(); i++) {
					if (IS_VALID(this->d_callbacks[i].get())) {
						this->d_callbacks[i]->do_call(args...);
					}
					else {
						this->d_callbacks.erase(this->d_callbacks.begin() + i);
						i--;
					};
				};
			};

			bool isValid() {
				return !isEmpty();
			};
			bool isEmpty() {
				return this->d_callbacks.empty();
			};
			void clear(INT_PTR id = 0) {
				if (id == 0) {
					this->d_callbacks.clear();
				}
				else {
					for (int i = 0; i < this->d_callbacks.size(); i++) {
						if (IS_VALID(this->d_callbacks[i].get())) {
							if (this->d_callbacks[i]->is_same(id)) {
								this->d_callbacks.erase(this->d_callbacks.begin() + i);
								i--;
							};
						}
						else {
							this->d_callbacks.erase(this->d_callbacks.begin() + i);
							i--;
						};
					};
				};
			};
		};

		typedef delegate<void(std::string message, EDebug type)> _OnInfo;

		// ----------------------------------------------------------------------------

#pragma endregion

#pragma region PARAMETERS

		typedef delegate<void(std::string message, EDebug type)> _OnInfo;

		static OnInfo onLog;

#pragma endregion

#pragma region MEMORY FUNCTIONS
	public:
		struct ALLOC_INFO
		{
			bool			readyToDelete = false;
			void* address = nullptr;
			unsigned int	size = 0;
			char			file[_MAX_PATH] = {};
			unsigned int	line = 0;
		};

	protected:
		static std::vector<ALLOC_INFO> g_LeakList;
		static std::mutex g_LeakMutex;

	public:
		YAMI_API static void* AddTrack(void* p, unsigned int asize, const char* fname, unsigned int lnum);
		YAMI_API static void ForceRemoveTrack(void* p);
		YAMI_API static void ForceRemoveTrackArray(void* p);
		YAMI_API static void RemoveTrack(void* p);
		YAMI_API static bool GarbageCollector();
		YAMI_API static void FreeAllMemory();

#pragma endregion

#pragma region STRING FUNCTIONS

		YAMI_API static std::string ToString(std::string val);

		YAMI_API static std::string ToString(const char* val);

		YAMI_API static std::string ToString(const wchar_t* val);

		YAMI_API static std::string ToString(std::wstring val);

		template<typename T>
		static auto ToString(std::vector<T> iterator) -> decltype(iterator.begin(), iterator.end(), std::string{}) {
			SAFE_EXCEPTION;
			DEBUG_TRY {
				std::ostringstream oss;
				bool first = true;
				for (const auto& entry : iterator)
				{
					if (first) {
						first = false;
					} else {
						oss << SEP_FOR_DEBUG;
					};
					oss << TO_STRING(entry);
				};

				return "("+oss.str()+")";
			}
			DEBUG_CATCH(...) {
			};
			return "";
		};

		template<typename T, std::size_t S>
		static auto ToString(std::array<T, S> iterator) -> decltype(iterator.begin(), iterator.end(), std::string{}) {
			SAFE_EXCEPTION;
			DEBUG_TRY {
				std::ostringstream oss;
				bool first = true;
				for (const auto& entry : iterator)
				{
					if (first) {
						first = false;
					} else {
						oss << SEP_FOR_DEBUG;
					};
					oss << TO_STRING(entry);
				};

				return "(" + oss.str() + ")";
			}
			DEBUG_CATCH(...) {
			};
			return "";
		};

		template<typename T>
		static auto ToString(std::list<T> iterator) -> decltype(iterator.begin(), iterator.end(), std::string{}) {
			SAFE_EXCEPTION;
			DEBUG_TRY {
				std::ostringstream oss;
				bool first = true;
				for (const auto& entry : iterator)
				{
					if (first) {
						first = false;
					} else {
						oss << SEP_FOR_DEBUG;
					};
					oss << TO_STRING(entry);
				};

				return "(" + oss.str() + ")";
			}
			DEBUG_CATCH(...) {
			};
			return "";
		};

		template<typename T, typename S>
		static auto ToString(std::map<T,S> iterator) -> decltype(iterator.begin(), iterator.end(), std::string{}) {
			SAFE_EXCEPTION;
			DEBUG_TRY {
				std::ostringstream oss;
				bool first = true;
				for (const auto& entry : iterator)
				{
					if (first) {
						first = false;
					} else {
						oss << SEP_FOR_DEBUG;
					};
					oss << TO_STRING(entry.first) << SEP_FOR_DEBUG << TO_STRING(entry.second);
				};

				return "(" + oss.str() + ")";
			}
			DEBUG_CATCH(...) {
			};
			return "";
		};
		
		template<typename T, typename S>
		static auto ToString(std::unordered_map<T, S> iterator) -> decltype(iterator.begin(), iterator.end(), std::string{}) {
			SAFE_EXCEPTION;
			DEBUG_TRY {
				std::ostringstream oss;
				bool first = true;
				for (const auto& entry : iterator)
				{
					if (first) {
						first = false;
					} else {
						oss << SEP_FOR_DEBUG;
					};
					oss << TO_STRING(entry.first) << SEP_FOR_DEBUG << TO_STRING(entry.second);
				};

				return "(" + oss.str() + ")";
			}
			DEBUG_CATCH(...) {
			};
			return "";
		};

		static std::string ToString(unsigned char val);

		static std::string ToStringHex(unsigned char val);

		template<typename T>
		static std::string ToString(T val) {
			SAFE_EXCEPTION;
			DEBUG_TRY {
				if (!IS_VALID(&val)) {
					return "";
				};
				std::ostringstream oss;
				oss << val;
				return oss.str();
			}
			DEBUG_CATCH(...) {

			};
			return "";
		};

		static std::string ToString(std::initializer_list<std::string> strList);

		template<typename... Args>
		static std::string ToString(const Args& ... args)
		{
			return TO_STRING({ TO_STRING(args)... });
		};

		YAMI_API static std::wstring ToWString(const wchar_t* val);

		YAMI_API static std::wstring ToWString(std::string val);

		YAMI_API static bool endsWith(std::string const& fullString, std::string const& ending);

#pragma endregion

#pragma region TIME FUNCTIONS
	protected:
		static std::clock_t initTime;
		static std::clock_t currentTime;

		static float deltaTime;
	public:
		YAMI_API static void CalculateDelta();

		YAMI_API static float GetDeltaTime();

		YAMI_API static int GetFPS();

#pragma endregion

#pragma region LOG FUNCTIONS
	protected:

		template<typename... Ts>
		static std::string DebugText(Ts... arguments) {
			std::string res = "";
			SAFE_EXCEPTION;
			DEBUG_TRY{
				res = TO_STRING(arguments...);
				if (ENABLE_DEBUG_LOG) {
					printf("%s", res.c_str());
				};
			}
			DEBUG_CATCH(...) {

			};
			return res;
		};

		template<typename Ts1, typename... Ts>
		static std::string DebugYAMI(Ts1 type, Ts... arguments) {
			std::time_t t = std::time(0);
			std::tm now;
			localtime_s(&now, &t);
			char buf[80];
			std::strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &now);
			return DebugText(buf, " - YAMI_", type, ": ", arguments..., "\n");
		};

		static FILE* pConsole;
	public:
		YAMI_API static void InitDebug();

		YAMI_API static void FinishDebug();

		YAMI_API static void OnLog(std::string text, EDebug type);

		template<typename... Ts>
		static void DebugException(Ts... arguments) {
			std::string txt = DebugYAMI("EXCEPTION", arguments...);
			ON_LOG(txt, EDebug::D_EXCEPTION);
		};

		template<typename... Ts>
		static void DebugError(Ts... arguments) {
			std::string txt = DebugYAMI("ERROR", arguments...);
			ON_LOG(txt, EDebug::D_ERROR);
		};

		template<typename... Ts>
		static void DebugWarning(Ts... arguments) {
			std::string txt = DebugYAMI("WARNING", arguments...);
			ON_LOG(txt, EDebug::D_WARNING);
		};

		template<typename... Ts>
		static void DebugInfo(Ts... arguments) {
			std::string txt = DebugYAMI("INFO", arguments...);
			ON_LOG(txt, EDebug::D_INFO);
		};

		template<typename... Ts>
		static void DebugSpam(Ts... arguments) {
			std::string txt = DebugYAMI("SPAM", arguments...);
			ON_LOG(txt, EDebug::D_SPAM);
		};

		template<typename... Ts>
		static void DebugNone(Ts... arguments) {
			std::string txt = DebugText(arguments..., "\n");
			ON_LOG(txt, EDebug::D_SPAM);
		};

#pragma endregion

#pragma region DATA FUNCTIONS

	public:
		template<typename T>
		static std::vector<T> SliceVector(std::vector<T> const& v, int pos, int size) {
			auto first = v.cbegin() + pos;
			auto last = v.cbegin() + pos + size;

			std::vector<T> vec(first, last);
			return vec;
		};

		YAMI_API static int ConcatData(uint8_t* data, unsigned int size);

#pragma endregion

#pragma region EXCEPTION AND VALID FUNCTIONS

	public:
#ifdef YAMI_WINDOWS

		YAMI_API static void excp_transl_func(unsigned int u, PEXCEPTION_POINTERS);

#endif

		YAMI_API static bool IsValid(INT_PTR ptr);

		template<typename T>
		static T* GetVPtr(T* ptr) {
			return *(T**)ptr;
		};

#pragma endregion

#pragma region KEYBOARD FUNCTIONS
#ifdef YAMI_WINDOWS //Only for Windows (for now)

	protected:
		static const int MAX_KEY_LENGTH;

		static bool isRecordedMouseMovement;

		static std::vector<std::pair<INPUT,INPUT>> recordedInputs;

		//static long initialTime;
		static std::chrono::time_point<std::chrono::steady_clock> initialTime;

	public:

		YAMI_API static bool IsKeyPressed(WORD key);

		YAMI_API static bool IsKeyReleased(WORD key);
		
		YAMI_API static std::vector<std::pair<INPUT, INPUT>> RecordKeyboardInput(bool recordMouseMovement = false, WORD endKey = VK_END);

		/*
		//For Games:
		//PlaybackKeyboardInput(-1, 100);
		//For Usual Input:
		//PlaybackKeyboardInput();
		*/
		YAMI_API static void PlaybackKeyboardInput(const std::vector<std::pair<INPUT, INPUT>> data = std::vector<std::pair<INPUT, INPUT>>(), long long timesToRepeat = -1, long long timeToUpKeyMS = -1, WORD endKey = VK_END);

#endif // YAMI_WINDOWS

#pragma endregion

#pragma region MAIN FUNCTIONS
	protected:
		static bool isInitialized;

	public:
		YAMI_API static void Init(bool _enableDebug, OnInfo _logDelegate) {

			SAFE_EXCEPTION;
			DEBUG_TRY{
				YAMIConfig::EnableDebug = _enableDebug;
#if defined(YAMI_WINDOWS)
				YAMIConfig::CurrentPlatform = EPlatform::PLATF_WINDOWS;
#elif defined(YAMI_ANDROID)
				YAMIConfig::CurrentPlatform = EPlatform::PLATF_ANDROID;
#elif defined(YAMI_LINUX)
				YAMIConfig::CurrentPlatform = EPlatform::PLATF_LINUX;
#elif defined(YAMI_MACOS)
				YAMIConfig::CurrentPlatform = EPlatform::PLATF_MACOS;
#elif defined(YAMI_IOS)
				YAMIConfig::CurrentPlatform = EPlatform::PLATF_IOS;
#endif
				onLog += _logDelegate;
				INIT_DEBUG;
				INIT_TIME;
				isInitialized = true;
			}
			DEBUG_CATCH(...) {

			}
		}

		YAMI_API static bool IsInitializated() {
			return isInitialized;
		}

		YAMI_API static void Update() {
			INIT_TIME;
		}

		YAMI_API static void End() {
			SAFE_EXCEPTION;
			DEBUG_TRY{
				onLog.clear();
				GARBAGE_COLLECTOR;
				FREE_MEMORY;
				FINISH_DEBUG;
				isInitialized = false;
			}
			DEBUG_CATCH(...) {

			}
		}

#pragma endregion
	};

	class YAMI_API Object {
	public:
#pragma region CONSTRUCTOR
		Object() {};
		virtual ~Object() {};
#pragma endregion

#pragma region OPERATORS
#if !defined(YAMI_MACOS)
		void operator delete(void* ptr);
#endif
#pragma endregion
	};
}