/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -m FLOAT, --umaxnorm=FLOAT  Umbral de l'autocorrelacion a largo plazo [default: 0.4]
    -n FLOAT, --u1norm=FLOAT    Umbral de l'autocorrelacion a corto plazo [default: 0.9]
    -p FLOAT, --upot=FLOAT      Umbral de la potencia [default: 0]
    -c FLOAT, --cclipping       Relative threshold used for central clipping pre-processing [default: 0.009]
    -e INT, --uext=INT          Umbral del numero de extremos relativos de la correlación [default: 100]
    -f INT, --medianfilt=INT    Number of errors that can be erased with a median filter (0 means no filter) [default: 0]
    -d INT, --downsampl=INT     Downsampling (+ LowPassFilter) factor (1 means no filter, d>=0) [default: 1]
    -w INT, --lpfwindow=INT     LPW function (0: rectangular, 1: triangular) [default: 1]
    -v, --verbose               For debugging purposes, shows on screen numbers and things while running the program
    -h, --help  Show this screen
    --version   Show the version of the project
    

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

bool verbose;

int main(int argc, const char *argv[]) {
	/// \TODO DOCOTP<br>
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.

  /// \DONE DOCOPT<br>
  /// Docopt called with many arguments
  ///
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

  // INICIALITZACIÓ VARIABLES FICADES ALS ARGUMENTS
	std::string input_wav   = args["<input-wav>"].asString();         ///< Input WAV File
	std::string output_txt  = args["<output-txt>"].asString();        ///< Output TXT File
  float umaxnorm          = stof(args["--umaxnorm"].asString()),    ///< Umbral de decisió pel valor de r[lag]/r[0]
        u1norm            = stof(args["--u1norm"].asString()),      ///< Umbral de decisió pel valor de r[1]/r[0]
        upot              = stof(args["--upot"].asString()),        ///< Umbral de decisió pel valor de la potencia
        cc                = stof(args["--cclipping"].asString());   ///< Variable que determines the value of the lindar in center-cliping, relative to the maximum of the signal.     
  int   uext              = stoi(args["--uext"].asString());        ///< Umbral de decisió pel nombre de extrems relatius de r[k]
  int   median_error      = stoi(args["--medianfilt"].asString()),  ///< Nombre d'errors que poden ser eliminats amb postprocessat per filtre de mediana (0 vol dir que no s'aplica filtre)
        lpfwindow         = stoi(args["--lpfwindow"].asString()),   ///< Tipus de filtre passa baixes (0 - rectangular, 1 - triangular)
        fN                = stoi(args["--downsampl"].asString());   ///< Factor de diezmado (1 quiere decir que no hay diezmado)
  //bool cc                 = args["--cclipping"].asBool();       
  verbose                 = args["--verbose"].asBool();             /// Chivatos y tal para ver que hacen las cosas

  int i, j;
  float aux;                 

  if (lpfwindow != 0 && lpfwindow != 1) lpfwindow = 1;
  if (fN < 1 || fN > 5) fN = 4;
  //fN = 1;

  // Provando - chivato per veure el valor de les variables
  if (verbose) {
    cout << "\n\n***************************************\nVariables d'arguments i els seus valors\n***************************************\n";
    for(auto const& arg : args) {
        std::cout << arg.first << ": " << arg.second << std::endl;
    }
  }

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  rate /= fN;
  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, umaxnorm, u1norm, upot, uext, PitchAnalyzer::RECT, 50, 500);

  // PREPROCESSING
  /// \TODO PRE-PROCESSING<br>
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  
  /// \DONE PRE-PROCESSING. Central-clipping<br>
  /// If central clipping is enabled (--cclipping=FLOAT, with FLOAT greater than 0), the signal will be central clipped.
  /// The threshold is relative to the maximum amplitud in the vector x.<br>
  /// Testing, it is seen a slight increase in the performance result. <br>
  /// Default lipping threshold is 0.009
  vector<float>::iterator iX;
  float max_x = *max_element(x.begin(), x.end());
  if (cc > 0) {                   
    if (verbose) cout << "\n\n**************\nCenter cliping**************\n\nBefore cc\tAfter cc\n";
    for(iX = x.begin(); iX < x.end(); iX++){
      if (verbose) cout << *iX << "\t";
      if ( (abs(*iX) / max_x ) < cc) *iX = 0;
//      if (abs(*iX) < ucc) *iX = 0;
      if (verbose) cout << *iX << "\n";
    }
  }

  /// \TODO Low pass filtering and down sampling

  /// \DONE Los pass filtering and down sampling
  /// Two filters have been added. Both filters are convolutional, and the length of the h[n] is determined by 'fN'.
  /// 
  /// * Rectangular filter.
  ///   Is a rectangular filter, the n values are multiplied by the same factor.
  /// * Triangular filter
  ///   Is a triangular filter, which takes fN-1 samples forward and fN-1 backward
  /// . 
  if (fN > 1) {
    int nouS = x.size()/fN;
    vector<float> x_ds(nouS); // X downsampled
    if (verbose) cout << "\n\n*******************\nLPF & down-sampling\n*******************\n";

    if(lpfwindow == 0) {
      // Filtre rectangular de N mostres
      cout.precision(6);
      
      if (verbose) cout << "FILTRE RECTANGULAR, n = " << fN << "\n";
      for(i = 0; i < nouS; i++){
        x_ds[i] = x[fN*i];
        if (verbose) cout << x[fN*i] << "\t";
        for (j = 1; j < fN; j++) {
          x_ds[i] += x[fN*i + j];
          if (verbose) cout << x[fN*i + j] << "\t";
        }
        x_ds[i] = x_ds[i] / fN;
        if (verbose) cout << ": " << x_ds[i] << "\n";
      }
    } else {
      //Filtre triangular de N mostres
      if (verbose) cout << "FILTRE TRIANGULAR, n = " << fN << "\n";
      for (i = 0; i < nouS; i++) {
        x_ds[i] = x[fN*i];
        if (verbose) cout << x[fN*i] << "\t";
        for (j = 1; j < fN; j++) {
          aux = (i>0)? x[fN*i + j] + x[fN*i - j] : x[fN*i + j];
          x_ds[i] += (1 - j / fN) * aux;
        }
        x_ds[i] /= (i>0)? fN : (fN+1)/2;
        if (verbose) cout << ": " << x_ds[i] << "\n";
      }

/*
      if (verbose) cout << "FILTRE TRIANGULAR, n = " << fN << "\n";
      for (i = 0; i < nouS; i++) {
        x_ds[i] = x[fN*i];
        if (verbose) cout << x[fN*i] << "\t";
        for (j = 1; j < fN; j++) {
          aux = (i>0)? x[fN*i + j] + x[fN*i - j] : x[fN*i + j];
          x_ds[i] += (1 - j / fN) * aux;
        }
        x_ds[i] /= (i>0)? fN : (fN+1)/2;
        if (verbose) cout << ": " << x_ds[i] << "\n";
      }
      */
    }
    x.resize(nouS);
    x = x_ds;
  }

  // PROCESSING
  /// Processing
  // f0 vector creation
  // Iterate for each frame and save values in f0 vector
  if (verbose) cout << "\n\n*****************\nProcessat\n*****************\nEXT\tr[1]/r[0]\tr[lag]/r[0]\tPot";
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }
  if (verbose) cout << "\n";
  // END PROCESSING



  // POST-PROCESSING
  /// \TODO Postprocessing
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.

  /// \DONE Median filter
  /// The signal is post-processed through a median filter. Calling the program you can choose the numbers of errors that the filter can fix.
  /// That means that if you have N values that are abruptly different from the signal, if it is set median_error=N, those errors would be vanished.
  /// That makes the filter length to be N*2+1.<br>
  /// Despite its complexity and our initial hope before testing, the results didn't come to be as expecteds.
  /// Then, it can be used but it won't be used as default.
  if (median_error > 0) {     // Si entra aqui vol dir que d'utilitza filtre de mediana
    if (verbose) cout << "\n*****************\nFiltre de mediana\n*****************"; //chivato filtre de mediana i tal

    int median_N = median_error * 2 + 1;  ///< Tamany del filtre de mediana per a corregir n errors
    vector<float> f0_aux = f0;            ///< Copia del vector f0 per a fer el processat
    float buffer_median[median_N];        ///< Buffer per a calcular la mediana, fem una especie de circular queue

    //S'inicialitzen els valors per a fer el filtre
    for (i = 0; i < median_error; i++)    // Els primers valors seran iguals (el filtre comença a la mostra número 'median_error')
      f0[i] = f0_aux[i];
    
    for (i = 0; i < median_N; i++)        // Coloquem els primers N valors a un vector auxiliar per a calcular la mediana
      buffer_median[i] = f0_aux[i];

    //Filtre de mediana comença
    float aux;
    for (i = median_error; i < f0.size() - median_error; i++) { // Recorrem el vector f0

      // Chivato pa ver que ordena bien
      if (verbose) { //primera part del chivato, el buffer de tamany median_N sense ordenar
        //cout << "\n\n" << i << "\n";
        cout << "\n";
        for (j=0; j<median_N ; j++)
          cout << buffer_median[j] << "\t";
      } //Just a chivato nvm

      j=0;
      while(j != median_N-1) {  /// \TODO Cambiar esto por un for iria bien?
        if(buffer_median[j] > buffer_median[j+1]){
          aux = buffer_median[j];                 // intercanvi de valors si no estan ordenats
          buffer_median[j] = buffer_median[j+1];
          buffer_median[j+1] = aux;
          if (j>0) j-=2;                          // si ha hagut canvi, torna enrere per veure si ha de tornar-la a canviar la que ha baixat
          //j = (j>0)? j-2 : j;
        } 
        j++;                   // si ja estaven ordenats, segueix avançant a ver que onda
      } // en principi esta ja el vector ordenat

      if (verbose) { // Segona part del chivato, el buffer ordenat
        cout << "\n";
        for (j=0; j<median_N ; j++)
          cout << buffer_median[j] << "\t";      
        cout << "\n";      
      } //Just a chivato
      f0[i] = buffer_median[median_error]; // Posem el valor corresponent al vector f0
      for(j = 0; j < median_N; j++)  // Tornem a crear el vector amb els valors directes del vector f0
        buffer_median[j] = f0_aux[i + j - median_error];
    }
  }
  // Aqui acaba el filtre de mediana, pa ir ubicaos i tal
  // END POST-PROCESSING                          
  if (verbose) cout << "\n\n*****************\nProcessat acabat!\n*****************\n\n";


  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}
