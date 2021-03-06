// Copyright 2004-present Facebook. All Rights Reserved.

#include "rsocket/statemachine/ChannelResponder.h"

namespace rsocket {

using namespace yarpl;
using namespace yarpl::flowable;

void ChannelResponder::onSubscribe(
    Reference<Subscription> subscription) noexcept {
  publisherSubscribe(std::move(subscription));
}

void ChannelResponder::onNext(Payload response) noexcept {
  checkPublisherOnNext();
  if (!publisherClosed()) {
    writePayload(std::move(response), false);
  }
}

void ChannelResponder::onComplete() noexcept {
  if (!publisherClosed()) {
    publisherComplete();
    completeStream();
    tryCompleteChannel();
  }
}

void ChannelResponder::onError(folly::exception_wrapper ex) noexcept {
  if (!publisherClosed()) {
    publisherComplete();
    applicationError(ex.get_exception()->what());
    tryCompleteChannel();
  }
}

void ChannelResponder::tryCompleteChannel() {
  if (publisherClosed() && consumerClosed()) {
    closeStream(StreamCompletionSignal::COMPLETE);
  }
}

void ChannelResponder::request(int64_t n) noexcept {
  ConsumerBase::generateRequest(n);
}

void ChannelResponder::cancel() noexcept {
  cancelConsumer();
  cancelStream();
  tryCompleteChannel();
}

void ChannelResponder::endStream(StreamCompletionSignal signal) {
  terminatePublisher();
  ConsumerBase::endStream(signal);
}

// TODO: remove this unused function
void ChannelResponder::processInitialFrame(Frame_REQUEST_CHANNEL&& frame) {
  onNextPayloadFrame(
      frame.requestN_,
      std::move(frame.payload_),
      frame.header_.flagsComplete(),
      true);
}

void ChannelResponder::handlePayload(
    Payload&& payload,
    bool complete,
    bool flagsNext) {
  onNextPayloadFrame(0, std::move(payload), complete, flagsNext);
}

void ChannelResponder::onNextPayloadFrame(
    uint32_t requestN,
    Payload&& payload,
    bool complete,
    bool next) {
  processRequestN(requestN);
  processPayload(std::move(payload), next);

  if (complete) {
    completeConsumer();
    tryCompleteChannel();
  }
}

void ChannelResponder::handleCancel() {
  publisherComplete();
  tryCompleteChannel();
}

void ChannelResponder::handleRequestN(uint32_t n) {
  processRequestN(n);
}

void ChannelResponder::handleError(folly::exception_wrapper ex) {
  errorConsumer(std::move(ex));
  tryCompleteChannel();
}
}
