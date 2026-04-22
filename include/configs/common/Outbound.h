#pragma once

#ifdef THRONE_CLI_HEADLESS

#include <QCoreApplication>
#define QApplication QCoreApplication
#include "third_party/Throne/include/configs/common/Outbound.h"
#undef QApplication

#else

#include "third_party/Throne/include/configs/common/Outbound.h"

#endif
