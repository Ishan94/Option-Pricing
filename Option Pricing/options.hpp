//
//  options.hpp
//  Option Pricing
//
//  Created by Ishan  on 08/10/22.
//

#ifndef options_hpp
#define options_hpp

#include <stdio.h>
#include <iostream>
#include <random>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <vector>
using namespace std;

class Option{
    /*
     Abstract Base class for Option
     */
protected:
    /* option attributes: Strike Price, Annualised vol, Expiry, and Risk Free Rate*/
    double Strike;
    double Sigma;
    double T;
    double RiskFreeRate;
    /* pure virtual function to compute the exercise value at nodes of the binomial tree */
    virtual double getCurrentExerciseValue(double spot, double t) = 0;
    /* virtual function to model a Binomial Tree for the american option */
    virtual double calcBinomialNodeValue(double spot, double t, double time_val);
    
public:
    Option(double strike, double sigma, double t, double riskFreeRate);
    /* pure virtual Functions which have  depending on option type: call/put and american/european */
    virtual double getValue(double spot, int dp) = 0;
    virtual double getIntrinsicValue(double spot) = 0;
    virtual double calcBlackScholesValue(double spot) = 0;
    double getDelta(double spot, int dp);
    double calcBinomialTreeValue(double spot, int treeDepth);
};


class EuropeanOption : public Option{
    /* Abstract base class for European Option */
    
protected:
    int dp;
public:
    EuropeanOption(double strike, double sigma, double t, double riskFreeRate, int dp);
    double getValue(double spot, int dp);
    double getCurrentExerciseValue(double spot, double t);
};


class AmericanOption : public Option{
    /* Abstract base class for American Option */
    
protected:
    int dp;
    
public:
    AmericanOption(double strike, double sigma, double t, double riskFreeRate, int dp);
    double getValue(double spot, int dp);
    double getCurrentExerciseValue(double spot, double t);
};


class EuropeanCall : public EuropeanOption{
    /* European Call Option class */
public:
    EuropeanCall(double strike, double sigma, double t, double riskFreeRate, int dp);
    double getIntrinsicValue(double spot);
    double calcBlackScholesValue(double spot);
};


class EuropeanPut : public EuropeanOption{
    /* European Put Option class */
public:
    EuropeanPut(double strike, double sigma, double t, double riskFreeRate, int dp);
    double getIntrinsicValue(double spot);
    double calcBlackScholesValue(double spot);
};


class AmericanCall : public AmericanOption{
    /* American Call Option class */
public:
    AmericanCall(double strike, double sigma, double t, double riskFreeRate, int dp);
    double getIntrinsicValue(double spot);
    double calcBlackScholesValue(double spot);
};


class AmericanPut : public AmericanOption{
    /* European Put Option class */
public:
    AmericanPut(double strike, double sigma, double t, double riskFreeRate, int dp);
    double getIntrinsicValue(double spot);
    double calcBlackScholesValue(double spot);
};


class Position{
    /* Class to model position of an option in a portfolio*/
public:
    double weight;
    Option* option;
    Position();
    Position(double w, Option* o);
};


class Portfolio{
  /*class to model a portfolio of options*/
protected:
    vector <Position> Positions;
    
public:
    Portfolio();
    void addPosition(const Position& pos);
    /*Function to get portfolio risk metrics*/
    double getValue(double spot);
    double getDelta(double spot);
    double calcDailyVaR(double spot, double sigma, double riskFreeRate, int N);
    double calcDailyExpectedShortfall(double spot, double sigma, double riskFreeRate, int N);
};

#endif /* options_hpp */
