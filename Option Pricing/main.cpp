//
//  main.cpp
//  Option Pricing
//
//  Created by Ishan  on 08/10/22.
//

#include <iostream>
#include "options.hpp"

void print_values(double spot, Option& opt, string desc, int x) {
cout << desc << opt.getValue(spot, x) << " Black Scholes: " <<
opt.calcBlackScholesValue(spot) << "\n"<<endl;
}


int main(int argc, const char * argv[]) {
    // insert code here...
    
    double test_spot = 50;
    double test_strike = 50;
    double test_r = 0.1;
    double test_t = 0.5;
    double test_sigma = 0.2;
    EuropeanCall test_ec1(test_strike, test_sigma, test_t, test_r, 20000);
    AmericanCall test_ac1(test_strike, test_sigma, test_t, test_r, 20000);
    EuropeanPut test_ep1(test_strike, test_sigma, test_t, test_r, 20000);
    AmericanPut test_ap1(test_strike, test_sigma, test_t, test_r, 20000);
    print_values(test_spot, test_ec1, "EUROPEAN CALL value : ", 20000);
    print_values(test_spot, test_ac1, "AMERICAN CALL value (CRR) : ", 20000);
    print_values(test_spot, test_ep1, "EUROPEAN PUT  value: ", 20000);
    print_values(test_spot, test_ap1, "AMERICAN PUT  value (CRR) : ", 20000);
    
    
    double spot = 100;
    double strike = 100;
    double r = 0.05;
    double t = 1;
    double sigma = 0.25;
    
    /* Creating ATM calls and puts on the same underlying
        volatility of the underlying = 25%
     */
    
    /* Depth of tree is set to 500 for portfolio risk analysis
    as we have around 20,000 spot price points; for faster computations*/
    
    EuropeanCall ec1(strike, sigma, t, r, 500);
    AmericanCall ac1(strike, sigma, t, r, 500);
    EuropeanPut ep1(strike, sigma, t, r, 500);
    AmericanPut ap1(strike, sigma, t, r, 500);
    
    
    Portfolio p;
    p.addPosition(Position(10, &ec1));
    p.addPosition(Position(10, &ac1));
    p.addPosition(Position(10, &ep1));
    p.addPosition(Position(10, &ap1));
    cout << endl ;
    cout << "Portfolio Value: " << p.getValue(100) << endl<<endl;
    cout << "Portfolio Delta: " << p.getDelta(100) << endl<<endl;
    
    cout<< "The portfolio Value at Risk at 95 percentile is "<< p.calcDailyVaR(spot, sigma, r, 20000)<<endl<<endl;
    
    cout<<"Expected Shortfall Measure of portfolio is "<<p.calcDailyExpectedShortfall(spot, sigma, r, 20000)<<endl;
    std::cout << "\n";
    return 0;
}
