/// @file

#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"

using namespace std;


/// Name space of UPC
namespace upc {
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {

  		/// \TODO Compute the autocorrelation r[l]

      /// \DONE Autocorrelation computated
      ///    - Autocorrelation set to 0
      ///    - Autocorrelation acumulated fot all the signal
      ///    - Autocorrelation divided by length
      ///    .
      /// Autocorrelation acabose conserva tus sueños, nuca sabes cuando te haran falta :)
    //Calcul de la autocorrelació
    for (unsigned int l = 0; l < r.size(); ++l) {
      r[l] = 0;
      for (unsigned int n = l; n < x.size(); n++) {
        r[l] += x[n] * x[n - l];
      }
      r[l] /= x.size();
    }

    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 
  }

  void PitchAnalyzer::set_window(Window win_type) {
    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type) {
    case HAMMING:
      /// \TODO Implement the Hamming window

      /// \DONE Hamming window implemented 
      window.assign(frameLen, 1);
      for(long unsigned int i=0;i<window.size();i++){  //hemos puesto long unsigned intpara que no salga un warning 
         window[i] = 0.53836+ 0.46164*cos(3.141592*((2*i)/frameLen-1));
      }
      break;
    case RECT:
    default:
      window.assign(frameLen, 1);
    }
  }

  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0) {
    npitch_min = (unsigned int) samplingFreq/max_F0;
    if (npitch_min < 2)
      npitch_min = 2;  // samplingFreq/2

    npitch_max = 1 + (unsigned int) samplingFreq/min_F0;

    //frameLen should include at least 2*T0
    if (npitch_max > frameLen/2)
      npitch_max = frameLen/2;
  }

  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const {
    /// \TODO Implement a rule to decide whether the sound is voiced or not.
    /// * You can use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.

    /// \DONE Implementat més o menys, cal seguir provant per trobar uns bons llindars.
    /// * Compara r[1]/r[0]
    /// * Compara r[lag]/r[0]
    /// * Compara potencia
    /// .
    /// No acaba d'anar bé :(

    cout.precision(6);
    cout << fixed << "\n" << r1norm << "\t" << rmaxnorm << "\t" << pot << "\t";
    if(r1norm > u1norm) return false;
    if(rmaxnorm > umaxnorm) return false;
    if(pot > upot) return false;
    cout << "sordo culero";
    return true;
  }

  float PitchAnalyzer::compute_pitch(vector<float> & x) const {
    //cout << "Se entra en la wea de computar el pich\n";
    if (x.size() != frameLen)
      return -1.0F;

    //Window input frame
    for (unsigned int i=0; i<x.size(); ++i)
      x[i] *= window[i];

    vector<float> r(npitch_max);

    //Compute correlation
    autocorrelation(x, r);

    vector<float>::const_iterator iR, iRMax;
    /*
    bool negatiu = false;
    int max;
    float valor_max;
    for(iR = r.begin();iR != r.end();iR++){
      if(negatiu == false && *iR<=0){
        negatiu = true;
      }
      if(*iR>valor_max){
        max = distance(r.begin(), iR); 
        valor_max  = *iR; 
      }
    }*/

    /// \TODO 
	/// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
	/// Choices to set the minimum value of the lag are:
	///    - The first negative value of the autocorrelation.
	///    - The lag corresponding to the maximum value of the pitch.
  ///	   .
	/// In either case, the lag should not exceed that of the minimum value of the pitch.

  /// \DONE
  /// Diria, no estic gaire segur
  /// Aviam, diria que troba bé el màxim, pero
  ///
  ///


  for(iR = iRMax = r.begin() + npitch_min; iR < r.begin() + npitch_max; iR++){
    if(*iR > *iRMax) iRMax = iR;
  }

  unsigned int lag = iRMax - r.begin();


  float pot = 10 * log10(r[0]);

    //You can print these (and other) features, look at them using wavesurfer
    //Based on that, implement a rule for unvoiced
    //change to #if 1 and compile
#if 0
    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << endl;
#endif
    
    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0]))
      return 0;
    else
      return (float) samplingFreq/(float) lag;
      // trama sonora, devolvemos la frecuencia del pitch
  }
}
