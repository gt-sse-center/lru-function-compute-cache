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

class kunsat_ips_key
{

	public:
        double Psi;
        double Th;
        double nVG;
        double alphaVG;
        double Ksat;

    	kunsat_ips_key(double psi, double th, double nvg, double alphavg, double ksat):
                Psi(psi),Th(th),nVG(nvg),alphaVG(alphavg),Ksat(ksat){}

  		bool operator<(const kunsat_ips_key &other) const
  		{

		if (Th < other.Th)
			return true;
		if (other.Th < Th)
				return false;

		if (Psi < other.Psi)
			return true;
		if (other.Psi < Psi)
				return false;

		if (nVG < other.nVG)
			return true;
		if (other.nVG < nVG)
				return false;

		if (alphaVG < other.alphaVG)
			return true;
		if (other.alphaVG < alphaVG)
				return false;
		
		return false;
	}
};

// We can use this for get_Theta() & get_dTheta_Psi() caches
class theta_ips_key
{

	public:
        double Psi;
        double nVG;
        double alphaVG;

    	theta_ips_key(double psi, double nvg, double alphavg):
                Psi(psi),nVG(nvg),alphaVG(alphavg){}

  		bool operator<(const theta_ips_key &other) const
  		{

		if (Psi < other.Psi)
			return true;
		if (other.Psi < Psi)
				return false;

		if (nVG < other.nVG)
			return true;
		if (other.nVG < nVG)
				return false;

		if (alphaVG < other.alphaVG)
			return true;
		if (other.alphaVG < alphaVG)
				return false;
		
		return false;
	}
};


class psi_ips_key
{

	public:
        double Th;
        double nVG;
        double alphaVG;

    	psi_ips_key(double Th, double nvg, double alphavg):
                Th(Th),nVG(nvg),alphaVG(alphavg){}

  		bool operator<(const psi_ips_key &other) const
  		{

		if (Th < other.Th)
			return true;
		if (other.Th < Th)
				return false;

		if (nVG < other.nVG)
			return true;
		if (other.nVG < nVG)
				return false;

		if (alphaVG < other.alphaVG)
			return true;
		if (other.alphaVG < alphaVG)
				return false;
		
		return false;
	}
};

template<typename key_t, typename value_t>
class tCache {
	public:
	typedef typename std::pair<key_t, value_t> key_value_pair_t;
	typedef typename std::list<key_value_pair_t>::iterator list_iterator_t;

	tCache(size_t csize)
	{
			_max_size = csize;
	}

	void put(const key_t& key, const value_t& value) {
		
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
	}

	const value_t& get(const key_t& key) {
		auto it = _cache_items_map.find(key);
		if (it == _cache_items_map.end()) {
			return defaulter;
		} else {
			_cache_items_list.splice(_cache_items_list.begin(), _cache_items_list, it->second);
		return it->second->second;
		}
	}

	bool exists(const key_t& key) const {
		bool res =  _cache_items_map.find(key) != _cache_items_map.end();
		return res;
	}

	size_t size() const {
		return _cache_items_map.size();
	}

private:
	std::list<key_value_pair_t> _cache_items_list;
	std::map<key_t, list_iterator_t> _cache_items_map;
	size_t _max_size = 0;
	value_t defaulter = -1;
};
#endif
