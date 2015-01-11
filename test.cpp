#include "mean_shift_clustering.h"
#include "measure_time.h"

#include <cstdlib>
#include <sstream>
#include <fstream>
#include <istream>
#include <chrono>
#include <boost/algorithm/string.hpp>

using namespace std;


int ReadDataFromFile(std::string filename, 
					 std::vector< std::vector<double> > &data,
					 int dim,
					 std::string splitchar=",");

void plot_data(FILE *gp,std::vector< std::vector<double> > &points,vector<int> &indices,bool is_save=false);

int main(void)
{
  string input  = "faithful.txt";
  string output = "clustered.txt";
 
  const int num_of_dimension = 2;
  const double clustering_threshold = 3e-3;
  int num_of_cluster = 0;

  ofstream outfile;
  outfile.open(output.c_str());

  vector< vector<double> > points;
  if(ReadDataFromFile(input, points, num_of_dimension) == 1){
	return -1;
  }

  MeanShiftClustering mean_shift_cluster(points, num_of_dimension);
  vector<int> indices;
  
  cout << "start clustering ..." << endl;
  MeasureTime timer;
  timer.start();
  num_of_cluster = mean_shift_cluster.Clustering(indices, clustering_threshold);
  timer.stop();
  std::cout << "処理時間:" << timer.getElapsedTime() << "[ms]" << std::endl;

  cout << "num of cluster : " << num_of_cluster << endl;

  for(int i = 0; i < num_of_cluster; i++){
	for(int j = 0; j < (int)points.size(); j++){
	  if(i == indices[j]){
		for(int k = 0; k < num_of_dimension; k++){
		  outfile << points[j][k] << " " << flush;
		}
		outfile << endl;
	  }
	}
	outfile << endl; outfile << endl;
  }
 
  FILE *gp = popen("gnuplot", "w");
  plot_data(gp, points, indices, true);
  pclose(gp);

  
  return 0;   
}
int ReadDataFromFile(string filename, 
					 vector< vector<double> > &data, 
					 int dim,
					 string splitchar)
{
  ifstream infile; 
  string tmpdata;
  vector<double> point(dim);
  vector<string> chardata;

  infile.open(filename.c_str());
  if (!infile)
	{
	  cout << "Unable to open input."<< endl;
	  return 1;
	}
  while(getline(infile, tmpdata)){
	boost::algorithm::split(chardata, tmpdata, boost::algorithm::is_any_of(splitchar.c_str()));
	for(int i = 0; i < dim; i++){
	  point[i] = stod(chardata[i]);
	}
	data.push_back(point);
  }

  infile.close();
  return 0;
} 
void plot_data(FILE *gp,std::vector< std::vector<double> > &points,vector<int> &indices,bool is_save)
{
  int n=*(max_element(indices.begin(),indices.end()))+1;
    
  fprintf(gp,"reset\n");
  if(is_save){
	fprintf(gp,"set terminal png\n");
	fprintf(gp,"set output \"output.png\"\n");
  }
  fprintf(gp,"set mouse\n");
  // fprintf(gp,"set size square\n");
  fprintf(gp,"set format x \"\"\n");
  fprintf(gp,"set format y \"\"\n");
  // fprintf(gp,"set xrange [-1.0:1.0]\n");
  // fprintf(gp,"set yrange [-1.0:1.0]\n");
  fprintf(gp,"plot '-' w p pt 7 ti 'class 1'");
  for(int i=1;i<n;i++){
	fprintf(gp,", '-' w p pt 7 ti 'class %d'",i+1);        
  }
  fprintf(gp,"\n");
    
  for(int cls=0;cls<n;cls++){
	for(int i=0;i<(int)points.size();i++){
	  if(indices[i]==cls) fprintf(gp,"%lf %lf\n",points[i][0],points[i][1]);
	}
	fprintf(gp,"e\n");
  }
    
  fflush(gp);
}
