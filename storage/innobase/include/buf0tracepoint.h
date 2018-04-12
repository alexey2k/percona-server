
#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER buf_tracepoint

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./buf_tracepoint.h"

#if !defined(BUF_TRACEPOINT_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define BUF_TRACEPOINT_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(buf_tracepoint, message,
        TP_ARGS(const char*, iotype, 
                int, value1, 
                int, value2),
        
        TP_FIELDS(ctf_string(iotype, iotype)
                  ctf_integer(int, value1, value1)
                  ctf_integer(int, value2, value2)))

TRACEPOINT_LOGLEVEL(buf_tracepoint, message, TRACE_WARNING)

#endif /* BUF_TRACEPOINT_H */

#include <lttng/tracepoint-event.h>
