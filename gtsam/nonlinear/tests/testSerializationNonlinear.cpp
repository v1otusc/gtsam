/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file testSerializationNonlinear.cpp
 * @brief
 * @author Richard Roberts
 * @date Feb 7, 2012
 */

#include <gtsam/nonlinear/Values.h>
#include <gtsam/nonlinear/ISAM2.h>
#include <gtsam/inference/Symbol.h>
#include <gtsam/geometry/PinholeCamera.h>
#include <gtsam/geometry/Pose2.h>
#include <gtsam/geometry/Cal3_S2.h>
#include <gtsam/geometry/Cal3DS2.h>
#include <gtsam/geometry/Cal3Bundler.h>

#include <gtsam/base/serializationTestHelpers.h>
#include <CppUnitLite/TestHarness.h>

using namespace std;
using namespace gtsam;
using namespace gtsam::serializationTestHelpers;


/* ************************************************************************* */
// Create GUIDs for Noisemodels
BOOST_CLASS_EXPORT_GUID(gtsam::noiseModel::Diagonal, "gtsam_noiseModel_Diagonal")
BOOST_CLASS_EXPORT_GUID(gtsam::noiseModel::mEstimator::Base , "gtsam_noiseModel_mEstimator_Base")
BOOST_CLASS_EXPORT_GUID(gtsam::noiseModel::mEstimator::Null , "gtsam_noiseModel_mEstimator_Null")
BOOST_CLASS_EXPORT_GUID(gtsam::noiseModel::mEstimator::Fair , "gtsam_noiseModel_mEstimator_Fair")
BOOST_CLASS_EXPORT_GUID(gtsam::noiseModel::mEstimator::Huber, "gtsam_noiseModel_mEstimator_Huber")
BOOST_CLASS_EXPORT_GUID(gtsam::noiseModel::mEstimator::Tukey, "gtsam_noiseModel_mEstimator_Tukey")
BOOST_CLASS_EXPORT_GUID(gtsam::noiseModel::Constrained, "gtsam_noiseModel_Constrained")
BOOST_CLASS_EXPORT_GUID(gtsam::noiseModel::Unit, "gtsam_noiseModel_Unit")
BOOST_CLASS_EXPORT_GUID(gtsam::noiseModel::Isotropic,"gtsam_noiseModel_Isotropic")
BOOST_CLASS_EXPORT_GUID(gtsam::noiseModel::Robust, "gtsam_noiseModel_Robust")
BOOST_CLASS_EXPORT_GUID(gtsam::SharedNoiseModel, "gtsam_SharedNoiseModel")
BOOST_CLASS_EXPORT_GUID(gtsam::SharedDiagonal, "gtsam_SharedDiagonal")

/* ************************************************************************* */
// Create GUIDs for factors
BOOST_CLASS_EXPORT_GUID(gtsam::PriorFactor<gtsam::Pose3>, "gtsam::PriorFactor<gtsam::Pose3>")
BOOST_CLASS_EXPORT_GUID(gtsam::JacobianFactor, "gtsam::JacobianFactor")
BOOST_CLASS_EXPORT_GUID(gtsam::HessianFactor , "gtsam::HessianFactor")
BOOST_CLASS_EXPORT_GUID(gtsam::GaussianConditional , "gtsam::GaussianConditional")


/* ************************************************************************* */
// Export all classes derived from Value
GTSAM_VALUE_EXPORT(gtsam::Cal3_S2)
GTSAM_VALUE_EXPORT(gtsam::Cal3Bundler)
GTSAM_VALUE_EXPORT(gtsam::Point3)
GTSAM_VALUE_EXPORT(gtsam::Pose3)
GTSAM_VALUE_EXPORT(gtsam::Rot3)
GTSAM_VALUE_EXPORT(gtsam::PinholeCamera<Cal3_S2>)
GTSAM_VALUE_EXPORT(gtsam::PinholeCamera<Cal3DS2>)
GTSAM_VALUE_EXPORT(gtsam::PinholeCamera<Cal3Bundler>)

namespace detail {
template<class T> struct pack {
 typedef T type;
};
}

/* ************************************************************************* */
typedef PinholeCamera<Cal3_S2>        PinholeCal3S2;
typedef PinholeCamera<Cal3DS2>        PinholeCal3DS2;
typedef PinholeCamera<Cal3Bundler>    PinholeCal3Bundler;

/* ************************************************************************* */
static Point3 pt3(1.0, 2.0, 3.0);
static Rot3 rt3 = Rot3::RzRyRx(1.0, 3.0, 2.0);
static Pose3 pose3(rt3, pt3);

static Cal3_S2 cal1(1.0, 2.0, 0.3, 0.1, 0.5);
static Cal3DS2 cal2(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
static Cal3Bundler cal3(1.0, 2.0, 3.0);

TEST (Serialization, TemplatedValues) {
  EXPECT(equalsObj(pt3));
  GenericValue<Point3> chv1(pt3);
  EXPECT(equalsObj(chv1));
  PinholeCal3S2 pc(pose3,cal1);
  EXPECT(equalsObj(pc));

  Values values;
  values.insert(1,pt3);

  EXPECT(equalsObj(values));
  values.insert(Symbol('a',0),  PinholeCal3S2(pose3, cal1));
  values.insert(Symbol('s',5), PinholeCal3DS2(pose3, cal2));
  values.insert(Symbol('d',47), PinholeCal3Bundler(pose3, cal3));
  values.insert(Symbol('a',5),  PinholeCal3S2(pose3, cal1));
  EXPECT(equalsObj(values));
  EXPECT(equalsXML(values));
  EXPECT(equalsBinary(values));
}

bool str_assert_equal(const string& expected, const string& actual) {
  if (actual == expected) return true;
  printf("Not equal:\n");
  std::cout << "expected:\n" << expected << std::endl;
  std::cout << "actual:\n" << actual << std::endl;
  return false;
}
TEST(Serialization, NoiseModelFactor1_backwards_compatibility) {
  PriorFactor<Pose3> factor(
      12345, Pose3(Rot3::RzRyRx(Vector3(1., 2., 3.)), Point3(4., 5., 6.)),
      noiseModel::Unit::Create(6));

  // String
  std::string actual_str = serialize(factor);
  // Generated from hash 3ba65669113f41d0e56a03b4b314047776bab5c4 (>4.2a4)
  std::string expected_str =
      "22 serialization::archive 19 1 0\n"
      "0 0 0 0 0 0 0 1 0 12345 0 1 6 21 gtsam_noiseModel_Unit 1 0\n"
      "1 1 0\n"
      "2 1 0\n"
      "3 0 0 0 0 6 0 1 0 0 0 6 1.00000000000000000e+00 1.00000000000000000e+00 "
      "1.00000000000000000e+00 1.00000000000000000e+00 1.00000000000000000e+00 "
      "1.00000000000000000e+00 6 1.00000000000000000e+00 "
      "1.00000000000000000e+00 1.00000000000000000e+00 1.00000000000000000e+00 "
      "1.00000000000000000e+00 1.00000000000000000e+00 1.00000000000000000e+00 "
      "1.00000000000000000e+00 0 0 0 0 4.11982245665682978e-01 "
      "-8.33737651774156818e-01 -3.67630462924899259e-01 "
      "-5.87266449276209815e-02 -4.26917621276207360e-01 "
      "9.02381585483330806e-01 -9.09297426825681709e-01 "
      "-3.50175488374014632e-01 -2.24845095366152908e-01 0 0 "
      "4.00000000000000000e+00 5.00000000000000000e+00 "
      "6.00000000000000000e+00\n";
  EXPECT(str_assert_equal(expected_str, actual_str));
  PriorFactor<Pose3> factor_deserialized_str = PriorFactor<Pose3>();
  deserializeFromString(expected_str, factor_deserialized_str);
  EXPECT(assert_equal(factor, factor_deserialized_str));

  // XML
  std::string actual_xml = serializeXML(factor);
  std::string expected_xml;
  {  // read from file
    // Generated from hash 3ba65669113f41d0e56a03b4b314047776bab5c4 (>4.2a4)
    std::ifstream f("priorFactor.xml");
    std::stringstream buffer;
    buffer << f.rdbuf();
    expected_xml = buffer.str();
  }
  EXPECT(str_assert_equal(expected_xml, actual_xml));
  PriorFactor<Pose3> factor_deserialized_xml = PriorFactor<Pose3>();
  deserializeFromXMLFile("priorFactor.xml", factor_deserialized_xml);
  EXPECT(assert_equal(factor, factor_deserialized_xml));
}

TEST(Serialization, ISAM2) {

  gtsam::ISAM2Params parameters;
  gtsam::ISAM2 solver(parameters);
  gtsam::NonlinearFactorGraph graph;
  gtsam::Values initialValues;
  initialValues.clear();

  gtsam::Vector6 temp6;
  for (int i = 0; i < 6; ++i) {
    temp6[i] = 0.0001;
  }
  gtsam::noiseModel::Diagonal::shared_ptr noiseModel = gtsam::noiseModel::Diagonal::Sigmas(temp6);

  gtsam::Pose3 pose0(gtsam::Rot3(), gtsam::Point3(0, 0, 0));
  gtsam::Symbol symbol0('x', 0);
  graph.add(gtsam::PriorFactor<gtsam::Pose3>(symbol0, pose0, noiseModel));
  initialValues.insert(symbol0, pose0);

  solver.update(graph, initialValues,
                gtsam::FastVector<gtsam::FactorIndex>());

  std::string binaryPath = "saved_solver.dat";
  try {
    std::ofstream outputStream(binaryPath);
    boost::archive::binary_oarchive outputArchive(outputStream);
    outputArchive << solver;
  } catch(...) {
    EXPECT(false);
  }

  gtsam::ISAM2 solverFromDisk;
  try {
    std::ifstream ifs(binaryPath);
    boost::archive::binary_iarchive inputArchive(ifs);
    inputArchive >> solverFromDisk;
  } catch(...) {
    EXPECT(false);
  }

  gtsam::Pose3 p1, p2;
  try {
    p1 = solver.calculateEstimate<gtsam::Pose3>(symbol0);
  } catch(std::exception &e) {
    EXPECT(false);
  }

  try {
    p2 = solverFromDisk.calculateEstimate<gtsam::Pose3>(symbol0);
  } catch(std::exception &e) {
    EXPECT(false);
  }
  EXPECT(assert_equal(p1, p2));
}

/* ************************************************************************* */
int main() { TestResult tr; return TestRegistry::runAllTests(tr); }
/* ************************************************************************* */
