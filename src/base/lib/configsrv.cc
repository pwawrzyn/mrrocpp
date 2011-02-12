/*!
 * @file configsrv.cc
 * @brief Class for configuration server - definitions
 *
 * @author Piotr Trojanek <piotr.trojanek@gmail.com>
 *
 * @ingroup LIB
 */

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "base/lib/configsrv.h"

configsrv::configsrv(const std::string & _dir, const std::string & ini_file) :
	dir(_dir)
{
	// Path to config file
	std::string file_location, common_file_location;

	// Build path to config file
	common_file_location = dir;
	common_file_location += "../configs/";
	common_file_location += "common.ini";

	// Read configuration
	read_property_tree_from_file(common_file_pt, common_file_location);

	if(ini_file.length()) {
		// Path to config file
		std::string file_location;

		// Build path to config file
		file_location = dir;
		file_location += ini_file;

		read_property_tree_from_file(file_pt, file_location);
	}
}

void configsrv::read_property_tree_from_file(boost::property_tree::ptree & pt, const std::string & file)
{
	std::cerr << "Reading config from: " << file << std::endl;
	try {
		if (boost::filesystem::extension(file) == ".ini") {
			boost::property_tree::read_ini(file, pt);
		} else if (boost::filesystem::extension(file) == ".xml") {
			boost::property_tree::read_xml(file, pt);
		} else {
			throw std::logic_error("unknown config file extension");
		}
	} catch (boost::property_tree::ptree_error & e) {
		std::cerr << e.what() << std::endl;
	}
}

void configsrv::change_ini_file(const std::string & ini_file)
{
	// Path to config files
	std::string file_location;

	// Stworzenie sciezki do pliku.
	file_location = dir;
	file_location += ini_file;

	// Reload configuration
	read_property_tree_from_file(file_pt, file_location);
}

std::string configsrv::value(const std::string & pt_path) const
{
	try {
		return file_pt.get <std::string> (pt_path);
	} catch (boost::property_tree::ptree_error & e) {
		return common_file_pt.get <std::string> (pt_path);
	}
}
