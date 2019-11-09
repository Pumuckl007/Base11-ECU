#include "miniunit.h"
#include "TEST_HELP_MACROS.h"
#include "Settings.h"
#include "Sampler.h"
#include "Arduino.h"
#include "PressureTransducer.h"

namespace RPL {
namespace SamplerTest{

class FakeSensor;
FakeSensor * fakeSensors[2];

class FakeSensor {
public:
  int pin;
  int id;
  int aquireCount;
  bool hasWritten;
  FakeSensor(int pin = 0, int id = 0) : pin(pin),
   id(id),
   aquireCount(0),
   hasWritten(false){
    if(!SamplerTest::fakeSensors[pin]){
      SamplerTest::fakeSensors[pin] = this;
    }
  };
  void aquire(){
    aquireCount++;
  }

  void resetAccumulator(){
    this->aquireCount = 0;
  }

  void writeValueToBuffer(char * buffer){
    buffer[0] = pin + '0';
    buffer[1] = id + '0';
    for(int i = 2; i<RPL::SCM_PACKET_LEN; i++){
      buffer[i] = aquireCount + '0';
    }
    this->hasWritten = true;
  }
};

MU_TEST(sampler_outputs_values_every_period){
  const int mapping[] = {0};
  Sampler<FakeSensor, 1> sampler(10, 2, mapping);
  MockSerial::resetSerials();
  Mocks::setAnalogWrite(0, 10);
  Mocks::setMillis(0);
  sampler.tick();
  mu_assert_int_eq(0, fakeSensors[0]->aquireCount);
  Mocks::setMillis(11);
  sampler.tick();

  mu_assert_int_eq(RPL::SCM_PACKET_LEN, Settings::FCB_STREAM->writeIndex);
  Settings::FCB_STREAM->writeBuffer[RPL::SCM_PACKET_LEN] = '\0';
  const char* expectedPacket = "001111111111";
  mu_assert_string_eq(expectedPacket, Settings::FCB_STREAM->writeBuffer);

  fakeSensors[0]->hasWritten = false;
  MockSerial::resetSerials();
  Mocks::setAnalogWrite(0, 11);
  Mocks::setMillis(21);
  sampler.tick();
  mu_check(fakeSensors[0]->hasWritten);
}

MU_TEST(sampler_aquires_values_every_period){
  const int mapping[] = {0};
  fakeSensors[0] = nullptr;
  Sampler<FakeSensor, 1> sampler(20, 20, mapping);
  MockSerial::resetSerials();
  Mocks::setAnalogWrite(0, 10);
  Mocks::setMillis(0);
  sampler.tick();
  mu_assert_int_eq(0, fakeSensors[0]->aquireCount);
  for(int i = 1; i<=19; i++){
    Mocks::setMillis(i);
    sampler.tick();
  }
  mu_assert_int_eq(19, fakeSensors[0]->aquireCount);
  Mocks::setMillis(20);
  sampler.tick();
  mu_assert_int_eq(RPL::SCM_PACKET_LEN, Settings::FCB_STREAM->writeIndex);
  mu_assert_int_eq('0' + 20,  Settings::FCB_STREAM->writeBuffer[2]);

}


MU_TEST(sampler_aquires_values_every_period_half_rate){
  const int mapping[] = {0};
  fakeSensors[0] = nullptr;
  Sampler<FakeSensor, 1> sampler(20, 2, mapping);
  MockSerial::resetSerials();
  Mocks::setAnalogWrite(0, 10);
  Mocks::setMillis(0);
  sampler.tick();
  mu_assert_int_eq(0, fakeSensors[0]->aquireCount);
  for(int i = 1; i<=19; i++){
    Mocks::setMillis(i);
    sampler.tick();
  }
  mu_assert_int_eq(1, fakeSensors[0]->aquireCount);
  Mocks::setMillis(20);
  sampler.tick();
  mu_assert_int_eq(RPL::SCM_PACKET_LEN, Settings::FCB_STREAM->writeIndex);
  mu_assert_int_eq('2',  Settings::FCB_STREAM->writeBuffer[2]);

}

MU_TEST(sampler_aquires_values_every_period_second_cycle){
  const int mapping[] = {0};
  fakeSensors[0] = nullptr;
  Sampler<FakeSensor, 1> sampler(20, 20, mapping);
  Mocks::setAnalogWrite(0, 10);
  Mocks::setMillis(10);
  sampler.tick();
  Mocks::setMillis(20);
  sampler.tick();
  MockSerial::resetSerials();

  mu_assert_int_eq(0, fakeSensors[0]->aquireCount);
  for(int i = 21; i<=39; i++){
    Mocks::setMillis(i);
    sampler.tick();
  }
  mu_assert_int_eq(19, fakeSensors[0]->aquireCount);
  Mocks::setMillis(40);
  sampler.tick();
  mu_assert_int_eq(RPL::SCM_PACKET_LEN, Settings::FCB_STREAM->writeIndex);
  mu_assert_int_eq('0' + 20,  Settings::FCB_STREAM->writeBuffer[2]);
}

MU_TEST(sampler_has_is_associated_with_sensors){
  const int mapping[] = {0, 2};
  Sampler<FakeSensor, 2> sampler(10, 2, mapping);
  MockSerial::resetSerials();
  Mocks::setAnalogWrite(0, 10);
  Mocks::setAnalogWrite(1, 10);
  Mocks::setMillis(0);
  sampler.tick();
  mu_assert_int_eq(0, fakeSensors[0]->aquireCount);
  Mocks::setMillis(11);
  sampler.tick();

  mu_assert_int_eq(RPL::SCM_PACKET_LEN*2, Settings::FCB_STREAM->writeIndex);
  Settings::FCB_STREAM->writeBuffer[RPL::SCM_PACKET_LEN*2] = '\0';
  const char* expectedPacket = "001111111111211111111111";
  mu_assert_string_eq(expectedPacket, Settings::FCB_STREAM->writeBuffer);

}

MU_TEST_SUITE(sampler_test){
  MU_RUN_TEST(sampler_outputs_values_every_period);
  MU_RUN_TEST(sampler_aquires_values_every_period);
  MU_RUN_TEST(sampler_aquires_values_every_period_half_rate);
  MU_RUN_TEST(sampler_aquires_values_every_period_second_cycle);
  MU_RUN_TEST(sampler_has_is_associated_with_sensors);
}
}
}
