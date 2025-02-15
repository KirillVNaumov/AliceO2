// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
#include "Framework/DataRefUtils.h"
#include "Framework/AlgorithmSpec.h"
#include "Framework/ControlService.h"
#include "Framework/ServiceRegistry.h"
#include "Framework/RawDeviceService.h"
#include "Framework/runDataProcessing.h"
#include "Framework/PluginManager.h"
#include <Monitoring/Monitoring.h>

using namespace o2::framework;

struct FakeCluster {
  float x;
  float y;
  float z;
  float q;
};

struct Summary {
  int inputCount;
  int clustersCount;
};

static bool wrapperCalled = false;

// This is how you can define your processing in a declarative way
std::vector<DataProcessorSpec> defineDataProcessing(ConfigContext const&)
{
  auto algo = adaptStateless([](DataAllocator& outputs, ControlService& control, RawDeviceService& service) {
    assert(wrapperCalled == true);
    // Creates a new message of size 1000 which
    // has "TPC" as data origin and "CLUSTERS" as data description.
    auto& tpcClusters = outputs.make<FakeCluster>(Output{"TPC", "CLUSTERS", 0}, 1000);
    int i = 0;

    for (auto& cluster : tpcClusters) {
      assert(i < 1000);
      cluster.x = i;
      cluster.y = i;
      cluster.z = i;
      cluster.q = i;
      i++;
    }

    auto& itsClusters = outputs.make<FakeCluster>(Output{"ITS", "CLUSTERS", 0}, 1000);
    i = 0;
    for (auto& cluster : itsClusters) {
      assert(i < 1000);
      cluster.x = i;
      cluster.y = i;
      cluster.z = i;
      cluster.q = i;
      i++;
    }
    control.readyToQuit(QuitRequest::All);
  });

  auto realAlgo = PluginManager::wrapAlgorithm(algo, [](AlgorithmSpec::ProcessCallback& callback, ProcessingContext& ctx) -> void {
    wrapperCalled = true;
    callback(ctx);
  });

  assert(wrapperCalled == false);
  return {
    DataProcessorSpec{
      "simple",
      Inputs{},
      {OutputSpec{"TPC", "CLUSTERS"},
       OutputSpec{"ITS", "CLUSTERS"}},
      realAlgo}};
}
