#include "miniunit.h"
#include "ThermoCouple.h"
#include "Adafruit_MAX31855.h"
#include "TEST_HELP_MACROS.h"
#include "../mocks/Arduino.h"

namespace RPL {
  namespace ThermoCoupleTest{
    MU_TEST(tc_reads_value_via_max31855_read){
      Mocks::resetPins();
      char buf[20];
      ThermoCouple tc(0, 1, 2, 3);
      Mocks::setThermoCoupleValue(2, 10.0);
      tc.aquire();
      mu_assert_double_eq(10.0, tc.writeValueToBuffer(buf));
      tc.resetAccumulator();
      Mocks::setThermoCoupleValue(2, 0.6);
      tc.aquire();
      mu_assert_double_eq(0.6, tc.writeValueToBuffer(buf));
    }

    MU_TEST(tc_discriminates_differnt_cs_pins){
      Mocks::resetPins();
      char buf[20];
      ThermoCouple tc0(0, 1, 0, 3);
      ThermoCouple tc1(0, 1, 1, 3);
      Mocks::setThermoCoupleValue(0, 10);
      Mocks::setThermoCoupleValue(1, 11);
      tc0.aquire();
      tc1.aquire();
      mu_assert_double_eq(10, tc0.writeValueToBuffer(buf));
      mu_assert_double_eq(11, tc1.writeValueToBuffer(buf));
    }

    MU_TEST(tc_writes_accurate_packet_for_id_zero){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "T0,10.00,59;";
      ThermoCouple tc(0, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, 10);
      tc.aquire();
      mu_assert_double_eq(10, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_writes_accurate_packet_for_id_one){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "T1,10.00,60;";
      ThermoCouple tc(1, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, 10);
      tc.aquire();
      mu_assert_double_eq(10, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_writes_accurate_packet_for_id_eleven){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "TB,10.00,77;";
      ThermoCouple tc(11, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, 10);
      tc.aquire();
      mu_assert_double_eq(10, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_writes_accurate_packet_for_ones_precision){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "TB,1.001,78;";
      ThermoCouple tc(11, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, 1.001);
      tc.aquire();
      mu_assert_double_eq(1.001, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_writes_accurate_packet_for_ones_precision_with_rounding){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "TB,1.001,78;";
      ThermoCouple tc(11, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, 1.0006);
      tc.aquire();
      mu_assert_double_eq(1.0006, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_writes_accurate_packet_for_ten_thousands_precision){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "TB,10006,85;";
      ThermoCouple tc(11, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, 10006);
      tc.aquire();
      mu_assert_double_eq(10006, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_writes_accurate_packet_for_ten_thousands_precision_with_round){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "TB,10000,79;";
      ThermoCouple tc(11, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, 9999.9);
      tc.aquire();
      mu_assert_int_eq(9999.9, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_writes_accurate_packet_for_tenths_precision){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "TB,.9001,86;";
      ThermoCouple tc(11, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, 0.9001);
      tc.aquire();
      mu_assert_double_eq(0.9001, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_averages_two_identical_samples){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "TB,1.000,77;";
      ThermoCouple tc(11, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, 1);
      tc.aquire();
      tc.aquire();
      mu_assert_double_eq(1, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_averages_two_dissimmilar_samples){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "TB,1.500,82;";
      ThermoCouple tc(11, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, 1);
      tc.aquire();
      Mocks::setThermoCoupleValue(0, 2);
      tc.aquire();
      mu_assert_double_eq(1.5, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_reports_zero_with_no_aquires){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "TB,.0000,76;";
      ThermoCouple tc(11, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, 1);
      mu_assert_double_eq(0, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_reports_negative_value){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "TB,-.100,74;";
      ThermoCouple tc(11, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, -0.1);
      tc.aquire();
      mu_assert_double_eq(-0.1, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST(tc_reports_negative_large_value){
      Mocks::resetPins();
      char buf[20];
      ZERO_MEM(buf);
      const char * expectedPacket = "TB,-10.0,74;";
      ThermoCouple tc(11, 1, 0, 3);
      Mocks::setThermoCoupleValue(0, -10.0);
      tc.aquire();
      mu_assert_double_eq(-10, tc.writeValueToBuffer(buf));
      mu_assert_string_eq(expectedPacket, buf);
    }

    MU_TEST_SUITE(thermo_couple_test){
      MU_RUN_TEST(tc_reads_value_via_max31855_read);
      MU_RUN_TEST(tc_discriminates_differnt_cs_pins);
      MU_RUN_TEST(tc_writes_accurate_packet_for_id_zero);
      MU_RUN_TEST(tc_writes_accurate_packet_for_id_one);
      MU_RUN_TEST(tc_writes_accurate_packet_for_id_eleven);
      MU_RUN_TEST(tc_writes_accurate_packet_for_ones_precision);
      MU_RUN_TEST(tc_writes_accurate_packet_for_ones_precision_with_rounding);
      MU_RUN_TEST(tc_writes_accurate_packet_for_ten_thousands_precision);
      MU_RUN_TEST(tc_writes_accurate_packet_for_ten_thousands_precision_with_round);
      MU_RUN_TEST(tc_writes_accurate_packet_for_tenths_precision);
      MU_RUN_TEST(tc_averages_two_identical_samples);
      MU_RUN_TEST(tc_averages_two_dissimmilar_samples);
      MU_RUN_TEST(tc_reports_zero_with_no_aquires);
      MU_RUN_TEST(tc_reports_negative_value);
      MU_RUN_TEST(tc_reports_negative_large_value);
    }
  }
}
