#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/exporters/ostream/span_exporter.h"

namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace nostd     = opentelemetry::nostd;

void InitTracer()
{
  auto exporter = std::unique_ptr<trace_sdk::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);
  auto processor = std::unique_ptr<trace_sdk::SpanProcessor>(
      new trace_sdk::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace_api::TracerProvider>(
      new trace_sdk::TracerProvider(std::move(processor)));

  // Set the global trace provider
  trace_api::Provider::SetTracerProvider(provider);
}

nostd::shared_ptr<trace_api::Tracer> GetTracer()
{
  auto provider = trace_api::Provider::GetTracerProvider();
  return provider->GetTracer("ot_seg_repr", OPENTELEMETRY_SDK_VERSION);
}

std::shared_ptr<opentelemetry::context::RuntimeContextStorage> storage_handle;

class ThreadPool {

public:

  ThreadPool() : worker_thread_([this] { WorkerLoop(); }) {

  }

  void WorkerLoop() {
    // This is what I would like to be able to do to keep the runtime context alive
    // until the thread is finished
    // auto ot_handle = opentelemetry::context::RuntimeContext::GetRuntimeContextStorage();

    // Uncomment this next line (and the two in main) to see the workaround demo
    // auto mock_handle = storage_handle;
    std::cout << "Starting worker" << std::endl;
    while(!finished_.load()) {
      auto scoped_span = trace_api::Scope(GetTracer()->StartSpan("task"));
      std::cout << "Doing work" << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Ending worker" << std::endl;
  }

  ~ThreadPool() {
    finished_.store(true);
    worker_thread_.join();
  }
  
private:

  std::thread worker_thread_;
  std::atomic<bool> finished_{false};
  
};

static std::unique_ptr<ThreadPool> global_thread_pool;

int main() {
  // Uncomment these next two lines (and the one in WorkerLoop) to see the workaround demo
  //  storage_handle = std::shared_ptr<opentelemetry::context::RuntimeContextStorage>(opentelemetry::context::GetDefaultStorage());
  //  opentelemetry::context::RuntimeContext::SetRuntimeContextStorage(storage_handle);
  InitTracer();
  global_thread_pool = std::make_unique<ThreadPool>();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  return 0;
}
