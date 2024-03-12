#include <iostream>
#include "YamiAPI.h"

using namespace YAMI;

int main()
{
    INIT_YAMI(true, [](std::string txt, EDebug deb) {});

    Utility::PlaybackKeyboardInput(Utility::RecordKeyboardInput(true));

    END_YAMI;
}

