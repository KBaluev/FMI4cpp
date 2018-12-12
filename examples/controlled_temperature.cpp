/*
 * The MIT License
 *
 * Copyright 2017-2018 Norwegian University of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING  FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <iostream>

#include <fmi4cpp/fmi2/fmi4cpp.hpp>
#include <fmi4cpp/tools/os_util.hpp>

using namespace std;
using namespace fmi4cpp::fmi2;

namespace logger = fmi4cpp::logger;

const double stop = 10.0;
const double step_size = 1E-4;
const fmi2ValueReference vr = 46;

int main() {

    const string fmu_path = string(getenv("TEST_FMUs"))
                            + "/2.0/cs/" + getOs() +
                            "/20sim/4.6.4.8004/ControlledTemperature/ControlledTemperature.fmu";

    auto fmu = Fmu(fmu_path).asCoSimulationFmu();

    for (const auto &v : *fmu->getModelDescription()->modelVariables()) {
        if (v.causality() == Causality::output) {
            logger::info("Name={}", v.name());
        }
    }

    auto slave = fmu->newInstance();
    slave->setupExperiment();
    slave->enterInitializationMode();
    slave->exitInitializationMode();

    clock_t begin = clock();
    
    double t;
    double ref;
    while ((t = slave->getSimulationTime()) <= (stop - step_size)) {
        if (!slave->doStep(step_size)) {
            logger::error("Error! doStep returned with status: {}", to_string(slave->getLastStatus()));
            break;
        }
        if (!slave->readReal(vr, ref)) {
            logger::error("Error! readReal returned with status: {}", to_string(slave->getLastStatus()));
            break;
        }
    }

    clock_t end = clock();

    long elapsed_ms =  (long) ((double(end-begin) / CLOCKS_PER_SEC) * 1000.0);
    logger::info("Time elapsed={}ms", elapsed_ms);

    slave->terminate();

}