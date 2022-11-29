//
//  options.cpp
//  Option Pricing
//
//  Created by Ishan  on 08/02/22.
//

#include "options.hpp"

double simStandardNormal() {
    static std::mt19937 mt_rand(122345);
    static std::normal_distribution<double> dis_normal(0.0, 1.0);
    return dis_normal(mt_rand);
}

/* Utility Function for phi(x) */
double normalCDF(double x){
    return std::erfc(-x / std::sqrt(2)) / 2;
}

Option::Option(double strike, double sigma, double t, double riskFreeRate): Strike(strike), Sigma(sigma), T(t), RiskFreeRate(riskFreeRate){}

/* Function to compute the exercise value at each node, option is exercised if the
   exercise value of option is greater than the discounted value of future option payoff
 */
double Option::calcBinomialNodeValue(double spot, double t, double time_val){
    return getCurrentExerciseValue(spot,t)>time_val?getCurrentExerciseValue(spot, t):time_val;
}

double Option::getDelta(double spot, int dp){
    /* Change in option value for 1% change in underlying */
    return getValue(1.01*spot, dp)-getValue(spot, dp);
}

double Option::calcBinomialTreeValue(double spot, int treeDepth){
    /* Implementation of Binomial Option Tree */
    double dt = T/treeDepth;
    double u = exp(Sigma*sqrt(dt));
    double d = 1/u;
    double p = (exp(RiskFreeRate*dt)-d)/(u-d);
    vector <double> vals;
    double s,t,v_time,v;
    
    /* Populate the Terminal Layer of expiry
       Maximum number of elements in the vector = Depth of the Binomial Tree = treeDepth + 1 */
    for (int i=0; i<= treeDepth; i++){
        s = spot*exp((2*i-treeDepth)*log(u));
        vals.push_back(getCurrentExerciseValue(s,T));
    }
    t = T;
    
    /* Traverse back through the tree */
    for (int j = treeDepth-1; j>=0; j--){
        t = t-dt;
        for( int i = 0; i<=j; i++){
            s = spot*exp((2*i-j)*log(u));
            /*  Compute the intrinsic value and time value at the node
                Time value is computed as PV of option payoffs one node ahead
                Intrinsic value is the exercise value at the node: Max(Spot-Stike,0) for calls and Max(Strike-Spot,0) for puts
             */
            
            /* To be efficient in implementation, we overwrite the vector created above at expiry as we only
                need to keep track of values in the jth node in order to traverse back and compute values at
                node j-1
             */
            v_time = exp(-RiskFreeRate*dt)*(p*vals[i+1] + (1-p)*vals[i]);
            v = calcBinomialNodeValue(s, t, v_time);
            vals[i] = v;
        }
        
    }
    return vals[0];
}

EuropeanOption::EuropeanOption(double strike, double sigma, double t, double riskFreeRate, int dp): Option(strike, sigma, t, riskFreeRate), dp(dp){}

double EuropeanOption::getValue(double spot, int dp){
    return calcBlackScholesValue(spot);
}

double EuropeanOption::getCurrentExerciseValue(double spot, double t){
    return t==T?getIntrinsicValue(spot):0;
}

AmericanOption::AmericanOption(double strike, double sigma, double t, double riskFreeRate, int dp): Option(strike, sigma, t, riskFreeRate), dp(dp){}

double AmericanOption::getValue(double spot, int dp){
    return calcBinomialTreeValue(spot, dp);
}

double AmericanOption::getCurrentExerciseValue(double spot, double t){
    return getIntrinsicValue(spot);
}

// Implementation of European Call
EuropeanCall::EuropeanCall(double strike, double sigma, double t, double riskFreeRate, int dp):EuropeanOption(strike, sigma, t, riskFreeRate, dp){};

double EuropeanCall::getIntrinsicValue(double spot){
    return max(spot-Strike,0.0);
}

double EuropeanCall::calcBlackScholesValue(double spot){
    double d1 = (1/(sqrt(T)*Sigma))*(((RiskFreeRate+Sigma*Sigma/2)*T)+(log(spot/Strike)));
    double d2 = d1 - Sigma*sqrt(T);
    return normalCDF(d1) * spot - normalCDF(d2) * Strike * exp(-RiskFreeRate * T);
}

// Implementation of European Put
EuropeanPut::EuropeanPut(double strike, double sigma, double t, double riskFreeRate, int dp):EuropeanOption(strike, sigma, t, riskFreeRate, dp){};

double EuropeanPut::getIntrinsicValue(double spot){
    return max(Strike-spot,0.0);
}

double EuropeanPut::calcBlackScholesValue(double spot){
    double d1 = (1/(sqrt(T)*Sigma))*(((RiskFreeRate+Sigma*Sigma/2)*T)+(log(spot/Strike)));
    double d2 = d1 - Sigma*sqrt(T);
    return (normalCDF(-d2) * Strike * exp(-RiskFreeRate * T) - normalCDF(-d1) * spot);
}


// Implementation of American Call
AmericanCall::AmericanCall(double strike, double sigma, double t, double riskFreeRate, int dp):AmericanOption(strike, sigma, t, riskFreeRate, dp){};

double AmericanCall::getIntrinsicValue(double spot){
    return max(spot-Strike,0.0);
}

double AmericanCall::calcBlackScholesValue(double spot){
    double d1 = (1/(sqrt(T)*Sigma))*(((RiskFreeRate+Sigma*Sigma/2)*T)+(log(spot/Strike)));
    double d2 = d1 - Sigma*sqrt(T);
    return normalCDF(d1) * spot - normalCDF(d2) * Strike * exp(-RiskFreeRate * T);
}

// Implementation of American Put
AmericanPut::AmericanPut(double strike, double sigma, double t, double riskFreeRate, int dp):AmericanOption(strike, sigma, t, riskFreeRate, dp){};

double AmericanPut::getIntrinsicValue(double spot){
    return max(Strike-spot,0.0);
}

double AmericanPut::calcBlackScholesValue(double spot){
    double d1 = (1/(sqrt(T)*Sigma))*(((RiskFreeRate+Sigma*Sigma/2)*T)+(log(spot/Strike)));
    double d2 = d1 - Sigma*sqrt(T);
    return (normalCDF(-d2) * Strike * exp(-RiskFreeRate * T) - normalCDF(-d1) * spot);
}


Position::Position(): weight(0.0), option(NULL){}

Position::Position(double w, Option* o) : weight(w), option(o){};

Portfolio::Portfolio(){};

void Portfolio::addPosition(const Position& pos){
    Positions.push_back(pos);
}

/* Function to compute the portfolio value */
double Portfolio::getValue(double spot){
    double value = 0.0;
    for(int i =0; i<Positions.size(); i++){
        value+= Positions.at(i).weight * Positions.at(i).option->getValue(spot, 500);
    }
    return value;
}

/* Function to compute the portfolio Delta = weighted average of option deltas */
double Portfolio::getDelta(double spot){
    double delta = 0.0;
    for(int i =0; i<Positions.size(); i++){
        delta += Positions.at(i).weight * Positions.at(i).option->getDelta(spot, 500);
    }
    return delta;
}


double Portfolio::calcDailyVaR(double spot, double sigma, double riskFreeRate, int N){
    double dt = 1.0/252.0;
    double vol = sigma*sqrt(dt);
    double drift = dt*(riskFreeRate - 0.5*vol*vol);
    vector <double> vals;
    /*
     Simulate N stock price scenarios assuming GBM
     Store the portfolio value at each stock price using functions described above
     */
    for(int i=1; i<=N; i++){
        double spot_new = spot * exp(drift + vol*simStandardNormal());
        vals.push_back(getValue(spot_new));
    }
    sort(vals.begin(),vals.end());
    /*
     Compute the worst 5% of the move
     */
    return getValue(spot) - vals.at(int(0.05*N-1));
}

double Portfolio::calcDailyExpectedShortfall(double spot, double sigma, double riskFreeRate, int N){
    double dt = 1.0/252.0;
    double vol = sigma*sqrt(dt);
    double drift = dt*(riskFreeRate - 0.5*vol*vol);
    vector <double> vals;
    for(int i=1; i<=N; i++){
        double spot_new = spot * exp(drift + vol*simStandardNormal());
        vals.push_back(getValue(spot_new));
    }
    sort(vals.begin(),vals.end());
    double expected_shortfall = getValue(spot);
    for(int i=0; i< (int)(0.05*N); i++){
        /* Compute the average of worst 5% moves and the difference is the expected shortfall */
        expected_shortfall -= vals.at(i)/(0.05*N);
    }
    return expected_shortfall;
}
