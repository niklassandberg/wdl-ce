#include "IPlugControlsDemo.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kNumParams
};

IPlugControlsDemo::IPlugControlsDemo(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kGain)->InitDouble("Gain", 0.0, -70.0, 12.0, 0.1, "dB");

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

IPlugControlsDemo::~IPlugControlsDemo() {}

void IPlugControlsDemo::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  //double samplesPerBeat = GetSamplesPerBeat();
  //double samplePos = (double) GetSamplePos();
  //double tempo = GetTempo();

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    *out1 = *in1 * mGain;
    *out2 = *in2 * mGain;
  }	
}

void IPlugControlsDemo::Reset()
{
  TRACE;
  IMutexLock lock(this);

  //double sr = GetSampleRate();
}

void IPlugControlsDemo::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kGain:
      mGain = GetParam(kGain)->DBToAmp();
      break;
      
    default:
      break;
  }
}


