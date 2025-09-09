#include <iostream>
#include <vector>

using namespace std;

double BuySellStock(const vector<double>& prices) {
    int left = 0;
    int right = 1;
    double max_profit = 0;
    while (right < prices.size()) {
        double delta = prices[right] - prices[left];
        if (delta > 0) {
            if (delta > max_profit) {
                max_profit = delta;
            }
        } else if (delta < 0) {
            left++;
            right = left;
        }
        right++;
    }

    return max_profit;
}

int main() {
    vector<double> stonks = {310, 315, 275, 295, 260, 270, 290, 230, 255, 250}; // 30 max profit

    double max_profit = BuySellStock(stonks);

    cout << "Max Profit for the day: " << max_profit << endl;

    return 0;
}