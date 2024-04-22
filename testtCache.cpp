#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "tCache.h"

using namespace std;
using namespace std::chrono;

tCache<kunsat_ips_key,double> cache(200);
double time_in_get_Kunsat = 0;
double time_in_cache = 0;


/* Original function for which compute cache is needed */
double get_Kunsat(double Th, double Psi, double nVG,double alphaVG, double Ksat, int &hit)
{
	double mpow, ahn, ahn_1, c1, c2;
double Kunsat;
	auto start = high_resolution_clock::now();
	mpow  = 1.0-1.0/nVG;
        ahn   = pow((alphaVG*Psi),nVG);
        ahn_1 = pow((alphaVG*Psi),(nVG-1.0));
        c1 = pow((1.0+ahn),(-mpow));
        c2 = pow((1.0+ahn),(0.5*mpow));
        Kunsat = Ksat*(1.0-ahn_1*c1)*(1.0-ahn_1*c1)/c2;
	auto stop = high_resolution_clock::now();
        duration<double, std::milli> ms_double = stop - start;
        //cout<< "Time in get_Kunsat: "<<ms_double.count()<<endl;
	time_in_get_Kunsat += ms_double.count();
	return Kunsat;
}



/* Cached function implementation */
double cached_getKunsat(double Th, double Psi, double nVG,double alphaVG, double Ksat, int &hit)
{
	double Kunsat = 0;
	auto start = high_resolution_clock::now();
	kunsat_ips_key lookupkey(Th,Psi,nVG,alphaVG,Ksat);
        if(cache.exists(lookupkey)){
        	Kunsat = cache.get(lookupkey);
		if(Kunsat == 0)
			cout << "Cached 0" <<endl;
                hit = 1;
		return Kunsat;
        }
		Kunsat = get_Kunsat(Th,Psi,nVG,alphaVG,Ksat,hit);
        cache.put(lookupkey,Kunsat);
		//cout << "Caching " <<Kunsat<<endl;
	hit = 0;
	auto stop = high_resolution_clock::now();
        duration<double, std::milli> ms_double = stop - start;
        //cout<< "Time in cache: "<<ms_double.count()<<endl;
	time_in_cache += ms_double.count();

	return Kunsat;

}

/* Pocess each line in the function trace */
int process_call_trace_line(std::string line, bool cacheEnabled, double &outkunsat)
{



	double Th, Psi, nVG,alphaVG,Ksat;
	double Kunsat = 0;
	int hit = 0;
	// parse line here & populate above
	
	vector<string> tokens;

	stringstream ss(line);
  	while (ss.good()) {
    		string token;
    		getline(ss, token, ',');
    		tokens.push_back(token);
  	}

	Th = stod(tokens[0]);
	Psi = stod(tokens[1]);
	nVG = stod(tokens[2]);
	alphaVG = stod(tokens[3]);
	Ksat = stod(tokens[4]);

	//cout << Th << "," << Psi << ", " << nVG << "," <<alphaVG << "," <<Ksat <<",";
	if(cacheEnabled){

		outkunsat = cached_getKunsat(Th,Psi,nVG,alphaVG,Ksat,hit);
	}
	else{
		outkunsat = get_Kunsat(Th,Psi,nVG,alphaVG,Ksat,hit);
	}
	


	return hit;
}

int main(int argc, char* argv[])
{
	int res = 0;
	double hit=0, miss=0;
	int num_lines = 0;
	bool cache_enabled = 0;
	double outkunsat = 0;
	
	// handle different command line arguements
	// nothing specified - run with cache disabled
	// 0 as cmd line arg - cache disabled
	// 1 as cmd line arg - cache enabled
	if (argc == 1)
        	std:cout<<"cache not specified - running without cache"<<endl;

	if (argc == 2) {
		std::cout<<"cache enabled: "<< argv[1] <<endl; 
		cache_enabled = true;
   	 }

	std::string line;
	std::ifstream infile("onehrfullprecisiontrace.txt");
	//std::ifstream infile("calltrace.txt");
	std::string outQualifier = cache_enabled?"cached":"nocache";
	std::string outFileName = std::string("calloutput").append(outQualifier);

	std::ofstream outputFile;
	outputFile.open(outFileName,std::ios::binary);
	for (; std::getline(infile,line) && num_lines < 100000; ++num_lines)
	{
		//cout << line <<endl;
		res = process_call_trace_line(line, cache_enabled, outkunsat);
		if(res == 0){
			miss++;
		}
		else{
			hit++;
		}
		if (outputFile.is_open())
			outputFile<<setprecision(14)<< outkunsat<<endl;
	}
	outputFile.close();
	std::cout<<endl<<"----------------------"<<endl;
	std::cout<<"Total calls: "<<num_lines<<endl;
	std::cout<<"Time spent in get_Kunsat: "<<time_in_get_Kunsat<<endl;;
	std::cout<<"Time spent in cache: "<<time_in_cache<<endl;
        std::cout<<"Total time: "<<time_in_cache+time_in_get_Kunsat<<endl;	
	std::cout<<"hit ratio: "<< hit/(num_lines)<<endl;
	std::cout<<"miss ratio: "<<miss/(num_lines) <<endl;
	std::cout<<endl<<"----------------------"<<endl;
}
