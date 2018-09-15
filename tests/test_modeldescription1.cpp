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

#define BOOST_TEST_MODULE ControlledTemperature_Modeldescription_Test

#include <string>
#include <boost/test/unit_test.hpp>
#include <fmicpp/tools/os_util.hpp>
#include <fmicpp/fmi2/fmicpp.hpp>


using namespace std;
using namespace fmicpp::fmi2;

const string fmu_path = string(getenv("TEST_FMUs"))
                        + "/FMI_2.0/CoSimulation/" + getOs() +
                        "/20sim/4.6.4.8004/ControlledTemperature/ControlledTemperature.fmu";


BOOST_AUTO_TEST_CASE(test1) {

    import::Fmu fmu(fmu_path);
    auto md = fmu.getModelDescription();
    auto md_cs = md.asCoSimulationModelDescription();

    BOOST_CHECK_EQUAL("2.0", md.fmiVersion());
    BOOST_CHECK_EQUAL("ControlledTemperature", md.modelName());

    BOOST_CHECK_EQUAL("{06c2700b-b39c-4895-9151-304ddde28443}", md.guid());
    BOOST_CHECK_EQUAL("{06c2700b-b39c-4895-9151-304ddde28443}", md_cs->guid());
    BOOST_CHECK_EQUAL("20-sim", md.generationTool());
    BOOST_CHECK_EQUAL("20-sim", md_cs->generationTool());

    BOOST_CHECK_EQUAL(true, md.supportsCoSimulation());
    BOOST_CHECK_EQUAL(false, md.supportsModelExchange());

    BOOST_CHECK_EQUAL(120, md.modelVariables().size());
    BOOST_CHECK_EQUAL(120, md_cs->modelVariables().size());

    auto heatCapacity1 = md.getVariableByName("HeatCapacity1.T0").asRealVariable();
    BOOST_CHECK_EQUAL(1, heatCapacity1.getValueReference());
    BOOST_CHECK_EQUAL(false, heatCapacity1.getMin().has_value());
    BOOST_CHECK_EQUAL(false, heatCapacity1.getMax().has_value());
    BOOST_CHECK_EQUAL(true, heatCapacity1.getStart().has_value());
    BOOST_CHECK_EQUAL(298.0, *heatCapacity1.getStart());
    BOOST_CHECK_EQUAL("starting temperature", heatCapacity1.getDescription());
    BOOST_CHECK_EQUAL(false, heatCapacity1.getQuantity().has_value());

    auto thermalConductor = md.getVariableByValueReference(12);
    BOOST_CHECK_EQUAL("TemperatureSource.T", thermalConductor.getName());
    BOOST_CHECK(fmi2Variability::tunable == thermalConductor.getVariability());
    BOOST_CHECK(fmi2Causality::parameter == thermalConductor.getCausality());

    auto sourceFiles = md_cs->sourceFiles();
    BOOST_CHECK_EQUAL(10, sourceFiles.size());
    BOOST_CHECK_EQUAL("EulerAngles.c", sourceFiles.at(0).name());

    auto outputs = md.modelStructure().outputs();
    BOOST_CHECK_EQUAL(2, outputs.size());
    BOOST_CHECK_EQUAL(115, outputs[0].index());
    BOOST_CHECK_EQUAL(116, outputs[1].index());

    auto de = md.defaultExperiment();
    BOOST_CHECK(de.has_value());
    BOOST_CHECK_EQUAL(0.0, *de->startTime());
    BOOST_CHECK_EQUAL(20.0, *de->stopTime());
    BOOST_CHECK_EQUAL(1E-4, *de->stepSize());
    BOOST_CHECK_EQUAL(false, de->tolerance().has_value());

}