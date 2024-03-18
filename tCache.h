#ifndef TCACHE_H
#define TCACHE_H

#include <unordered_map>
#include <list>
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <map>
#include <memory>
#include <chrono>

using namespace std;
using namespace std::chrono;

class fn_ips
{

	public:
        double Psi;
        double Th;
        double nVG;
        double alphaVG;
        double Ksat;

        fn_ips(double psi, double th, double nvg, double alphavg, double ksat):
                Psi(psi),Th(th),nVG(nvg),alphaVG(alphavg),Ksat(ksat)
        {}
  bool operator==(const fn_ips &other) const
  { 
	  //std::cout<<"[tCache] Operator =="<<endl; 
	  return (Psi == other.Psi
            && Th == other.Th
            && nVG == other.nVG
            && alphaVG == other.alphaVG);
            //&& Ksat == other.Ksat);
  }

  bool operator<(const fn_ips &other) const
  {

	  //std::cout<<"[tCache] Operator <"<<endl;
	  return (Th < other.Th);
	  bool res = true; 
	  if (Th < other.Th)
		  return res;
	  else if (Psi < other.Psi)
		  return res;
	  else if (nVG < other.nVG)
		  return res;
	  else if (alphaVG < other.alphaVG)
		  return res;

	  return false;
  }
};

class fn_ips_hasher
{
  public:
  std::size_t operator()(const fn_ips& k) const
  {
    std::size_t result = 0;
    std::hash<double> hasher;
    using std::hash;

  // picked from Boost::hascombine
  //  result ^= hasher(k.Psi) + 0x9e3779b9 + (result << 6) + (result>>2);
  //  result ^= hasher(k.Th) + 0x9e3779b9 + (result << 6) + (result>>2);
  //  result ^= hasher(k.nVG) + 0x9e3779b9 + (result << 6) + (result>>2);
  //  result ^= hasher(k.alphaVG) + 0x9e3779b9 + (result << 6) + (result>>2);
    
    // below is naive - see if this is ok??
    // Compute individual hash values for first,
    // second and third and combine them using XOR
    // and bit shifting:
    result =  (hash<double>()(k.Psi) ^
    		hash<double>()(k.Th) ^
    		hash<double>()(k.nVG) ^
    		hash<double>()(k.alphaVG));
		//hash<double>(k.nVG)
    return result;
  }

};

//template<typename key_t, typename value_t,typename key_hasher_t>
template<typename key_t, typename value_t>//,typename key_hasher_t>
class tCache {
	public:
	typedef typename std::pair<key_t, value_t> key_value_pair_t;
	typedef typename std::list<key_value_pair_t>::iterator list_iterator_t;

	tCache()
	{
	}

	void put(const key_t& key, const value_t& value) {
		
		//std::cout <<"[tCache] put"<<std::endl;
		//auto start = high_resolution_clock::now();
		auto it = _cache_items_map.find(key);
		_cache_items_list.push_front(key_value_pair_t(key, value));
		if (it != _cache_items_map.end()) {
			_cache_items_list.erase(it->second);
			_cache_items_map.erase(it);
		}
		_cache_items_map[key] = _cache_items_list.begin();

		if (_cache_items_map.size() > _max_size) {
			auto last = _cache_items_list.end();
			last--;
			_cache_items_map.erase(last->first);
			_cache_items_list.pop_back();
		}
                //auto stop = high_resolution_clock::now();
                //duration<double, std::milli> ms_double = stop - start;
		//std::cout<< "[tCache] put: "<<ms_double.count()<<endl;
	}

	const value_t& get(const key_t& key) {
		//std::cout <<"[tCache] get"<<std::endl;
		//auto start = high_resolution_clock::now();
		auto it = _cache_items_map.find(key);
		if (it == _cache_items_map.end()) {
			//throw std::range_error("[tCache] Unknown Issue");
			return defaulter;
		} else {
			_cache_items_list.splice(_cache_items_list.begin(), _cache_items_list, it->second);
                //auto stop = high_resolution_clock::now();
                //duration<double, std::milli> ms_double = stop - start;
		//std::cout<< "[tCache] get: "<<ms_double.count()<<endl;
		return it->second->second;
		}
	}

	bool exists(const key_t& key) const {
		
		auto start = high_resolution_clock::now();
		bool res =  _cache_items_map.find(key) != _cache_items_map.end();
                auto stop = high_resolution_clock::now();
                duration<double, std::milli> ms_double = stop - start;
		//std::cout<< "[tCache] exists: "<<ms_double.count()<<endl;
		return res;
	}

	size_t size() const {
		return _cache_items_map.size();
	}

private:
	std::list<key_value_pair_t> _cache_items_list;
	std::map<key_t, list_iterator_t> _cache_items_map;
	const size_t _max_size = 200;
	value_t defaulter;
};
#endif
