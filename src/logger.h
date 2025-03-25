#include <raylib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#ifndef LOGGER_H_
#define LOGGER_H_

typedef enum LogLevel {
  LOG_LEVEL_ERR,
  LOG_LEVEL_WARN,
  LOG_LEVEL_INFO,
  LOG_LEVEL_TRACE,
  LOG_LEVEL_DEBUG,
} LogLevel;

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif // LOG_LEVEL

#define _RED "\033[31m"
#define _YLW "\033[33m"
#define _BLU "\033[34m"
#define _CYN "\033[36m"
#define _GRY "\033[90m"
#define _RST "\033[0m"

#define _LBL_ERR "[ERROR] "
#define _LBL_WARN "[WARN]  "
#define _LBL_INFO "[INFO]  "
#define _LBL_TRACE "[TRACE] "
#define _LBL_DEBUG "[DEBUG] "

#define _FMT(color, label, fmt) color label fmt _RST "\n"

void get_current_time(char formatted_time[20]) {
  time_t now = time(NULL);
  struct tm *local_time = localtime(&now);
  strftime(formatted_time, 20, "%Y-%m-%d %H:%M:%S", local_time);
}

#define LOG(stream, fmt, ...)                                                  \
  {                                                                            \
    char ftime[20];                                                            \
    get_current_time(ftime);                                                   \
    fprintf(stream, _GRY "%s " _RST fmt, ftime, ##__VA_ARGS__, __FILE__,       \
            __LINE__);                                                         \
  }

#if LOG_LEVEL >= 0
#define LERROR(fmt, ...)                                                       \
  LOG(stderr, _FMT(_RED, _LBL_ERR, fmt " (%s:%d)"), ##__VA_ARGS__)
#else
#define LERROR(fmt, ...)
#endif

#if LOG_LEVEL >= 1
#define LWARN(fmt, ...) LOG(stdout, _FMT(_YLW, _LBL_WARN, fmt), ##__VA_ARGS__)
#else
#define LWARN(fmt, ...)
#endif

#if LOG_LEVEL >= 2
#define LINFO(fmt, ...) LOG(stdout, _FMT(_BLU, _LBL_INFO, fmt), ##__VA_ARGS__)
#else
#define LINFO(fmt, ...)
#endif

#if LOG_LEVEL >= 3
#define LTRACE(fmt, ...) LOG(stdout, _FMT(_CYN, _LBL_TRACE, fmt), ##__VA_ARGS__)
#else
#define LTRACE(fmt, ...)
#endif

#if LOG_LEVEL >= 4
#define LDEBUG(fmt, ...) LOG(stdout, _FMT(_GRY, _LBL_DEBUG, fmt), ##__VA_ARGS__)
#else
#define LDEBUG(fmt, ...)
#endif

void CustomRaylibLogCallback(int logLevel, const char *text, va_list args) {
  char msg[1000];
  vsprintf(msg, text, args);
  switch (logLevel) {
  case LOG_DEBUG:
    LDEBUG("[RAYLIB] %s", msg);
    break;
  case LOG_ERROR:
    LERROR("[RAYLIB] %s", msg);
    break;
  case LOG_TRACE:
    LTRACE("[RAYLIB] %s", msg);
    break;
  case LOG_WARNING:
    LWARN("[RAYLIB] %s", msg);
    break;
  case LOG_INFO:
  default:
    LINFO("[RAYLIB] %s", msg);
    break;
  }
}

#endif // LOGGER_H_