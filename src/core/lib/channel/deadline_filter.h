//
// Copyright 2016, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef GRPC_CORE_LIB_CHANNEL_DEADLINE_FILTER_H
#define GRPC_CORE_LIB_CHANNEL_DEADLINE_FILTER_H

#include "src/core/lib/channel/channel_stack.h"
#include "src/core/lib/iomgr/timer.h"

// State used for filters that enforce call deadlines.
// Must be the first field in the filter's call_data.
typedef struct grpc_deadline_state {
  // We take a reference to the call stack for the timer callback.
  grpc_call_stack* call_stack;
  // Guards access to timer_pending and timer.
  gpr_mu timer_mu;
  // True if the timer callback is currently pending.
  bool timer_pending;
  // The deadline timer.
  grpc_timer timer;
  // Closure to invoke when the call is complete.
  // We use this to cancel the timer.
  grpc_closure on_complete;
  // The original on_complete closure, which we chain to after our own
  // closure is invoked.
  grpc_closure* next_on_complete;
} grpc_deadline_state;

// To be used in a filter's init_call_elem(), destroy_call_elem(), and
// start_transport_stream_op() methods to enforce call deadlines.
//
// REQUIRES: The first field in elem->call_data is a grpc_deadline_state.
//
// For grpc_deadline_state_client_start_transport_stream_op(), it is the
// caller's responsibility to chain to the next filter if necessary
// after the function returns.
void grpc_deadline_state_init(grpc_exec_ctx* exec_ctx, grpc_call_element* elem,
                              grpc_call_element_args* args);
void grpc_deadline_state_destroy(grpc_exec_ctx* exec_ctx,
                                 grpc_call_element* elem);
void grpc_deadline_state_client_start_transport_stream_op(
    grpc_exec_ctx* exec_ctx, grpc_call_element* elem,
    grpc_transport_stream_op* op);

// Deadline filters for direct client channels and server channels.
// Note: Deadlines for non-direct client channels are handled by the
// client_channel filter.
extern const grpc_channel_filter grpc_client_deadline_filter;
extern const grpc_channel_filter grpc_server_deadline_filter;

#endif /* GRPC_CORE_LIB_CHANNEL_DEADLINE_FILTER_H */
