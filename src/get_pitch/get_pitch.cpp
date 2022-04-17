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
    -n FLOAT, --u1norm==FLOAT   Umbral de l'autocorrelacion a corto plazo [default: 1.0]
    -p FLOAT, --upot==FLOAT     Umbral de la potencia [default: -20]
    -e INT, --uext=INT          Umbral del numero de extremos [default:21]
    -c, --cclipping             Wether it does central clipping or not
    -h, --help  Show this screen
    --version   Show the version of the project
    

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.

  /// \DONE Docotopt
  /// We 
  ///
  ///
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  cout << "arxius bé\n";
  float umaxnorm = stof(args["--umaxnorm"].asString()),
        u1norm   = stof(args["--u1norm"].asString()),
        upot     = stof(args["--upot"].asString()),
        uext     = stof(args["--uext"].asString());
  cout << "constants inicials bé\n";
  //Provando
  for(auto const& arg : args) {
      std::cout << arg.first << ": " << arg.second << std::endl;
  }
  bool cc = args["--cclipping"].asBool();

  //bool  cc = (args["--cc"].asString() == "TRUE")? true : false;
  //cout << cc << "\n\n";
  //int idk;
  //cin >> idk;

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

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  
  /// \DONE Central-clipping
  /// If central clipping is enabled (--cclipping has been called), the signal will be central clipped, clipping threshold is 2.5e-4
  vector<float>::iterator iX;
  if (cc) {
    float ucc = 0.00025;
    for(iX = x.begin(); iX < x.end(); iX++){
      cout << *iX << "\t";
      if (abs(*iX) < ucc) *iX = 0;
      cout << *iX << "\n";
    }
  }


  // Iterate for each frame and save values in f0 vector
  //vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.

  /// \DONE Median filter
  /// Median filter, diria, no ho se, em fa por
  /// S'ha de testejar i tal yatusae
  int i, j=0;
  int median_error = 2;                 /// Nombre d'errors que pot corregir el filtre de mediana
  int median_N = median_error * 2 + 1;  /// Tamany del filtre de mediana per a corregir n errors
  vector<float> f0_aux = f0;            /// Copia del vector f0 per a fer el processat
  float buffer_median[median_N];        /// Buffer per a calcular la mediana, fem una especie de circular queue

  for (i = 0; i < median_error; i++) //Els primers valors seran iguals (el filtre comença a la mostra número 'median_error')
    f0[i] = f0_aux[i];
  
  for (i = 0; i < median_N; i++){  //Coloquem els primers N valors a un vector auxiliar per a calcular la mediana
    buffer_median[i] = f0_aux[i];
  }

  //Filtre de mediana comença
  int k=0; /// Posició del nou element en el buffer
  float aux;
  for (i = median_error; i < f0.size() - median_error - 1; i++) { //Recorrem el vector f0
    //for(j = 0; j < median_N; j++) { //Ordenem el buffer, bucle de cops que recorre el buffer
    for(int j = 0; j < median_N - 1; j++){ /// \TODO Substituir aixo per un whil i probarho que funcioni bé, fer proves al MATLAB
    while(j != median_N-1) {
      if(buffer_median[j] > buffer_median[j+1]){
        aux = buffer_median[j];                 //intercanvi de valors si no estan ordenats
        buffer_median[j] = buffer_median[j+1];
        buffer_median[j+1] = aux;
        j--;                          //si ha hagut canvi, torna enrere per veure si ha de tornar-la a canviar la que ha baixat
      } else{j++;}                    //si ja estaven ordenats, segueix avançant a ver que onda
    } //en principi esta ja el vector ordenat
    f0[i] = buffer_median[median_error]; //Posem el valor corresponent al vector f0
    for(j=i-median_error; j<i+median_error;j++) //Tornem a crear el vector amb els valors directes del vector f0
      buffer_median[j] = f0_aux[j];
  }

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
