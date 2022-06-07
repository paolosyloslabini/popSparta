#include <poplar/DeviceManager.hpp>
#include <poplar/Engine.hpp>
#include <poplar/Graph.hpp>
#include <poplar/DebugContext.hpp>



#include <popops/codelets.hpp>

#include <poplin/codelets.hpp>
#include <poplin/MatMul.hpp>

#include <popsparse/codelets.hpp>

#include <algorithm>
#include <iostream>

using namespace poplar;

int main() {
  // Create the DeviceManager which is used to discover devices
  auto manager = DeviceManager::createDeviceManager();

  // Attempt to attach to a single IPU:
  auto devices = manager.getDevices(poplar::TargetType::IPU, 1);
  std::cout << "Trying to attach to IPU\n";
  auto it = std::find_if(devices.begin(), devices.end(), [](Device &device) {
     return device.attach();
  });

  if (it == devices.end()) {
    std::cerr << "Error attaching to device\n";
    return 1; //EXIT_FAILURE
  }

  auto device = std::move(*it);
  std::cout << "Attached to IPU " << device.getId() << std::endl;

  Target target = device.getTarget();

  // Create the Graph object
  Graph graph(target);


  //adding libraries codelets
  popops::addCodelets(graph);
  poplin::addCodelets(graph);
  popsparse::addCodelets(graph);

  poplar::Tensor matrix_A;
  poplar::Tensor matrix_B;

  int mat_N = 1000;
  std::size_t mat_size = sizeof(int)*mat_N;
  const std::vector<std::size_t> shape = {mat_size, mat_size};
  const poplar::Type poptype = FLOAT;

  matrix_A = poplin::createMatMulInputLHS(graph, poptype, shape, shape, DebugContext{});
  matrix_B = poplin::createMatMulInputRHS(graph, poptype, shape, shape, DebugContext{});

  //TODO
  //set tile mapping (fill the tensors)
}