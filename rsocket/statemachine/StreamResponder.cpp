// Copyright 2004-present Facebook. All Rights Reserved.

#include "rsocket/statemachine/StreamResponder.h"

namespace rsocket {

using namespace yarpl;
using namespace yarpl::flowable;

void StreamResponder::onSubscribe(
    Reference<yarpl::flowable::Subscription> subscription) noexcept {
  publisherSubscribe(std::move(subscription));
}

void StreamResponder::onNext(Payload response) noexcept {
  checkPublisherOnNext();
  if (!publisherClosed()) {
    writePayload(std::move(response), false);
  }
}

void StreamResponder::onComplete() noexcept {
  if (!publisherClosed()) {
    publisherComplete();
    completeStream();
    closeStream(StreamCompletionSignal::COMPLETE);
  }
}

void StreamResponder::onError(folly::exception_wrapper ex) noexcept {
  if (!publisherClosed()) {
    publisherComplete();
    applicationError(ex.get_exception()->what());
    closeStream(StreamCompletionSignal::ERROR);
  }
}

void StreamResponder::endStream(StreamCompletionSignal signal) {
  terminatePublisher();
  StreamStateMachineBase::endStream(signal);
}

void StreamResponder::handleCancel() {
  closeStream(StreamCompletionSignal::CANCEL);
  publisherComplete();
}

void StreamResponder::handleRequestN(uint32_t n) {
  processRequestN(n);
}
}
