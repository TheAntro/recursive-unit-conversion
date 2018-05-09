/*
 * Author: Valtteri Ranta: valtterih.ranta@gmail.com
 *
 * This is a conversion program that takes units to convert from the user and initial value
 * The program then converts the value to goal unit using its database of conversion rates between units
 *
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <stdlib.h>
#include "console.h"
#include "simpio.h"

//excuse for using namespace std: this is a school project
//actually a reimplementation of the final project for
//Programming Basics course of Tampere University of Technology
using namespace std;

const string fileconstant = "rates.txt";

/**
 * @brief initializeRates
 * @param filename
 * @return vector<string> with lines from filename as values
 */
vector<string> initializeRates(string filename);

/**
 * @brief initializeMap
 * @param lineVector vector which includes a datafile line tokenized to a vector
 * @return map<string, map<string, double>> where each unit from the file is paired with a map with direct conversion units and rates of the unit
 */
map<string, map<string, double> > initializeMap(vector<string> lineVector);

/**
 * @brief stringsFromLine tokenizes the line into strings, using ; as token splitter
 * @param line
 * @return vector<string> with tokens
 */
vector<string> stringsFromLine(string line);

/**
 * @brief recConversion Recursive backtracking algorithm for searching a conversion path between two units
 * @param currentUnit starts with the initial unit, moves as algorithm seeks the path
 * @param goalUnit stays the same the entire time, as the goal unit
 * @param visited tracks which units have been visited
 * @param rateMap the database for conversion rates
 * @return true if conversion path exist, false otherwise
 */
bool recConversion(string currentUnit, string goalUnit, vector<string> & visited, map<string, map<string, double>> & rateMap);

/**
 * @brief existsInVector checks if a string exist in a vector
 * @param test string to be searched from the vector
 * @param testVector vector to be searched
 * @return true if string is found in the vector, false otherwise
 */
bool existsInVector(string test, vector<string> testVector);

/**
 * @brief conversion a wrapper and a printer function for converting the starting unit to the goal unit
 * @param value value of the conversion in initial units
 * @param startingUnit initial unit
 * @param goalUnit unit to convert into
 * @param rateMap data structure for conversion rates
 */
void conversion(double value, string startingUnit, string goalUnit, map<string, map<string, double>> & rateMap);

/**
 * @brief calculateConversion calculates the final conversion
 * @param startVal initial value
 * @param route route along which the conversion is made
 * @param rateMap datastructure for the conversion rates
 * @return double result of the conversion from starting to goal units
 */
double calculateConversion(double startVal, vector<string> &route, map<string, map<string, double> > &rateMap);

int main() {
    vector<string> rateVector = initializeRates(fileconstant);
    map<string, map<string, double> > rateMap = initializeMap(rateVector);
    string startingUnit = "";
    string goalUnit = "";
    while (rateMap.find(startingUnit) == rateMap.end()) {
        cout << "Enter starting unit: ";
        startingUnit = getLine();
    }
    while (rateMap.find(goalUnit) == rateMap.end()) {
        cout << "Enter goal unit: ";
        goalUnit = getLine();
    }
    cout << "Enter value for conversion: ";
    double value = getDouble();
    conversion(value, startingUnit, goalUnit, rateMap);
    return 0;
}

/**
 * @brief initializeRates
 * @param filename
 * @return vector<string> with lines from filename as values
 */
vector<string> initializeRates(string filename) {
    vector<string> rates;
    ifstream infile;
    infile.open(filename);
    if (infile.fail()) {
        cout << "could not open file" << endl;
        infile.clear();
        infile.close();
        return rates;
    }
    while (!infile.fail()) {
        string line = "";
        getline(infile, line);
        rates.push_back(line);
    }
    infile.close();
    return rates;
}

/**
 * @brief initializeMap
 * @param lineVector vector which includes a datafile line tokenized to a vector
 * @return map<string, map<string, double>> where each unit from the file is paired with a map with direct conversion units and rates of the unit
 */
map<string, map<string, double> > initializeMap(vector<string> lineVector) {
    map<string, map<string, double> > data;
    for (int i = 0; i < lineVector.size()-1; i++) {
        string line = lineVector[i];
        vector<string> units = stringsFromLine(line);
        string unit1 = units[0];
        string unit2 = units[1];
        double rate = atof(units[2].c_str());
        data[unit1][unit2] = rate;
        data[unit2][unit1] = 1/rate;
    }
    return data;
}

/**
 * @brief stringsFromLine tokenizes the line into strings, using ; as token splitter
 * @param line
 * @return vector<string> with tokens
 */
vector<string> stringsFromLine(string line) {
    vector<string> tokens;
    string currentToken = "";
    for (int i = 0; i < line.length(); i++) {
        if (line[i] == ';') {
            tokens.push_back(currentToken);
            currentToken = "";
        } else {
            currentToken += line[i];
        }
    }
    //Last token ends in newline, not in ;, so add the last token into the vector
    tokens.push_back(currentToken);
    return tokens;
}

/**
 * @brief recConversion Recursive backtracking algorithm for searching a conversion path between two units
 * @param currentUnit starts with the initial unit, moves as algorithm seeks the path
 * @param goalUnit stays the same the entire time, as the goal unit
 * @param visited tracks which units have been visited
 * @param rateMap the database for conversion rates
 * @return true if conversion path exist, false otherwise
 */
bool recConversion(string currentUnit, string goalUnit, vector<string> & visited, map<string, map<string, double>> & rateMap) {
    map<string, double> toTest = rateMap.at(currentUnit);
    // if goalUnit is in the direct conversion rates of the unit under investigation, conversion path has been found
    if (toTest.find(goalUnit) != toTest.end()) {
        visited.push_back(currentUnit);
        visited.push_back(goalUnit);
        return true;
    }
    // if the current unit under investigation has already been visited, the recursion has started going in cycles and should stop
    if (existsInVector(currentUnit, visited)) return false;
    visited.push_back(currentUnit);
    for (const auto &key : toTest) {
        currentUnit = key.first;
        if (recConversion(currentUnit, goalUnit, visited, rateMap)) {
            return true;
        }
    }
    //remove wrong inclusions while backtracking (NOT OPTIONAL, if removed, conversion path will sometimes include traces of wrong units
    // crashing the program)
    visited.pop_back();
    //backtracking, paths exhausted, return false
    return false;
}

/**
 * @brief existsInVector checks if a string exist in a vector
 * @param test string to be searched from the vector
 * @param testVector vector to be searched
 * @return true if string is found in the vector, false otherwise
 */
bool existsInVector(string test, vector<string> testVector) {
    for (int i = 0; i < testVector.size(); i++) {
        if (testVector[i] == test) return true;
    }
    return false;
}

/**
 * @brief conversion a wrapper and a printer function for converting the starting unit to the goal unit
 * @param value value of the conversion in initial units
 * @param startingUnit initial unit
 * @param goalUnit unit to convert into
 * @param rateMap data structure for conversion rates
 */
void conversion(double startVal, string startingUnit, string goalUnit, map<string, map<string, double>> & rateMap) {
    // this vector is passed as a reference to the recursive algorithm, and keeps track of the conversion route
    vector<string> routeVec;
    if (recConversion(startingUnit, goalUnit, routeVec, rateMap)) {
        // if conversion route was found, routeVec will contain it as the recursive function returns true
        double result = calculateConversion(startVal, routeVec, rateMap);
        cout << startVal << " " << startingUnit << " = " << result << " " << goalUnit << endl;
    } else {
        cout << "conversion route not found" << endl;
    }
}

/**
 * @brief calculateConversion calculates the final conversion
 * @param startVal initial value
 * @param route route along which the conversion is made
 * @param rateMap datastructure for the conversion rates
 * @return double result of the conversion from starting to goal units
 */
double calculateConversion(double startVal, vector<string> & route, map<string, map<string, double>> & rateMap) {
    double result = startVal;
    map<string, double> innermap;
    // initial value is converted through each step of the conversion path, ending to the correct final value
    for(int i = 0; i < route.size()-1; i++) {
        innermap = rateMap.at(route[i]);
        double rate = innermap.at(route[i+1]);
        result *= rate;
    }
    return result;
}
