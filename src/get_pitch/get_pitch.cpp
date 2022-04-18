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
    -m FLOAT, --umaxnorm=FLOAT  Umbral de l'autocorrelacion a largo plazo [default: 0.49]
    -n FLOAT, --u1norm=FLOAT    Umbral de l'autocorrelacion a corto plazo [default: 1.0]
    -p FLOAT, --upot=FLOAT      Umbral de la potencia [default: 0]
    -e INT, --uext=INT          Umbral del numero de extremos [default: 21]
    -f INT, --medianfilt=INT    Number of errors that can be erased with a median filter (0 means no filter) [default: 0]
    -c, --cclipping             Wether it does central clipping or not
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
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.

  /// \DONE Docotopt
  /// Docopt called with many arguments
  ///
  ///
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav   = args["<input-wav>"].asString();         /// Input WAV File
	std::string output_txt  = args["<output-txt>"].asString();        /// Output TXT File
  float umaxnorm          = stof(args["--umaxnorm"].asString()),    /// Umbral de decisió pel valor de r[lag]/r[0]
        u1norm            = stof(args["--u1norm"].asString()),      /// Umbral de decisió pel valor de r[1]/r[0]
        upot              = stof(args["--upot"].asString());        /// Umbral de decisió pel valor de la potencia
  int   uext              = stoi(args["--uext"].asString());        /// Umbral de decisió pel nombre de extrems relatius de r[k]
  int   median_error      = stoi(args["--medianfilt"].asString());  /// Nombre d'errors que poden ser eliminats amb postprocessat per filtre de mediana (0 vol dir que no s'aplica filtre)
  bool cc                 = args["--cclipping"].asBool();           /// Variable que determines wether center-cliping will be applied or not.
  verbose                 = args["--verbose"].asBool();             /// Chivatos y tal para ver que hacen las cosas

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

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate,umaxnorm, u1norm, upot, uext,PitchAnalyzer::HAMMING, 50, 500);

  // PREPROCESSING
  /// \TODO Preprocessing
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  
  /// \DONE Central-clipping
  /// If central clipping is enabled (--cclipping has been called), the signal will be central clipped, clipping threshold is 2.5e-4
  vector<float>::iterator iX;
  if (cc) {                   
    if (verbose) cout << "\n\n**************\nCenter cliping**************\n\nBefore cc\tAfter cc\n";
    float ucc = 0.00025;
    for(iX = x.begin(); iX < x.end(); iX++){
      if (verbose) cout << *iX << "\t";
      if (abs(*iX) < ucc) *iX = 0;
      if (verbose) cout << *iX << "\n";
    }
  }

  /// \TODO Low pass filtering and down sampling



  // PROCESSING
  /// Processing
  // f0 vector creation
  // Iterate for each frame and save values in f0 vector
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }


  // POST-PROCESSING
  /// \TODO Postprocessing
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.

  /** Chivato(?) - f0 abans del filtre de mediana 
  cout << "\n\nf0 antes\n";
  for(int i=0; i<f0.size() ; i++)
    cout << f0[i] << "\n"; **/

  /// \DONE Median filter
  /// Median filter, diria, no ho se, em fa por
  /// S'ha de testejar i tal yatusae
  /// Cagun deu l'he fet pel mati tot d'una tirada i no he compilat res, casi em compila a la priemra i no se com va aah
  if (median_error > 0) {
    int i, j=0;                 
    int median_N = median_error * 2 + 1;  /// Tamany del filtre de mediana per a corregir n errors
    vector<float> f0_aux = f0;            /// Copia del vector f0 per a fer el processat
    float buffer_median[median_N];        /// Buffer per a calcular la mediana, fem una especie de circular queue

    if (verbose) cout << "\n\n*****************\nFiltre de mediana\n*****************"; //chivato filtre de mediana i tal
    for (i = 0; i < median_error; i++) //Els primers valors seran iguals (el filtre comença a la mostra número 'median_error')
      f0[i] = f0_aux[i];
    
    for (i = 0; i < median_N; i++)  //Coloquem els primers N valors a un vector auxiliar per a calcular la mediana
      buffer_median[i] = f0_aux[i];

    //Filtre de mediana comença
    float aux;
    for (i = median_error; i < f0.size() - median_error; i++) { //Recorrem el vector f0
      //if (verbose) cout.precision(2);
      //Chivato pa ver que ordena bien
      if (verbose) { //primera part del chivato, el buffer de tamany median_N sense ordenar
        //cout << "\n\n" << i << "\n";
        cout << "\n";
        for (j=0; j<median_N ; j++)
          cout << buffer_median[j] << "\t";
      }
      j=0;
      while(j != median_N-1) {
        if(buffer_median[j] > buffer_median[j+1]){
          aux = buffer_median[j];                 //intercanvi de valors si no estan ordenats
          buffer_median[j] = buffer_median[j+1];
          buffer_median[j+1] = aux;
          if (j>0) j-=2;                          //si ha hagut canvi, torna enrere per veure si ha de tornar-la a canviar la que ha baixat
          //j = (j>0)? j-2 : j;
        } 
        j++;                   //si ja estaven ordenats, segueix avançant a ver que onda
      } //en principi esta ja el vector ordenat

      if (verbose) { //Segona part del filtre, el buffer ordenat
        cout << "\n";
        for (j=0; j<median_N ; j++)
          cout << buffer_median[j] << "\t";
      
        cout << "\n";      
      }

      f0[i] = buffer_median[median_error]; //Posem el valor corresponent al vector f0
  //    for(j=i-median_error; j<i+median_error;j++) 
      for(j = 0; j < median_N; j++)  //Tornem a crear el vector amb els valors directes del vector f0
        buffer_median[j] = f0_aux[i + j - median_error];
    }
  }

  /** Chivato (?)
  cout << "\n\nDespués: \n";
  for(i=0; i<f0.size() ; i++)
    cout << f0[i] << "\n";
  **/


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
