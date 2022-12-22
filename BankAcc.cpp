#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <map>
#include <iomanip>

using std::string;

void deposit(double &balance,
             double amount) {  // Modifies the balance to reflect a deposit
  balance += amount;
}

bool withdraw(double &balance,
              double amount) {  // Withdraws money from balance if the amount
                                // requested is less than the balance
  if (balance >= amount) {
    balance -= amount;
    return true;
  }
  return false;
}

void overdraft(
    double &balance,
    double amount) {  // Withdraws requested amount from balance including a fee
  balance -= amount;
  balance -= 35;
}

double interest_for_month(double balance,
                          double apr) {  // Calculates the interest generated
                                         // per month based on principle and apr
  double pr = apr / 12;  // Because their are 12 months, the apr is divided by
                         // 12 to find the rate for any given month
  pr = pr / 100;
  double interest = balance * pr;
  interest = interest * 100;
  interest = floor(interest) / 100;
  return interest;
}

void string_date_to_int_ptrs(
    string date, int *yr, int *m,
    int *d) {  // Dereferences pointed values and assigns them to date elements
  char delim = '-';
  int spot = 0;  // The location of '-' character
  string chunk;  // Smaller piece from the bigger string
  int count = 0;
  while ((spot = static_cast<int>(date.find(delim))) != string::npos) {
    chunk = date.substr(0, spot);
    if (count == 0) {
      *yr = stoi(chunk);
      count++;
      date.erase(0, spot + 1);
    } else if (count == 1) {
      *m = stoi(chunk);
      count++;
      date.erase(0, spot + 1);
    }
  }
  *d = stoi(date);
}

void string_date_to_ints(string date, int &yr, int &m,
                         int &d) {  // Creates pointers for date elements.
  char delim = '-';
  int spot = 0;
  string chunk;
  int count = 0;
  int *pt_yr, *pt_m, *pt_d;
  while ((spot = static_cast<int>(date.find(delim))) != string::npos) {
    chunk = date.substr(0, spot);
    if (count == 0) {
      yr = stoi(chunk);
      pt_yr = &yr;
      count++;
      date.erase(0, spot + 1);
    } else if (count == 1) {
      m = stoi(chunk);
      pt_m = &m;
      count++;
      date.erase(0, spot + 1);
    }
  }
  d = stoi(date);
  pt_d = &d;
  string_date_to_int_ptrs(date, pt_yr, pt_m, pt_d);
}

int firsts_calculation(int start_year, int start_month, int start_day,
                       int end_year, int end_month,
                       int end_day) {  // Calculates the number of firsts of the
                                       // month between two dates
  int year_diff = end_year - start_year;
  int max_firsts =
      year_diff *
      12;  // The highest number of firsts possible based soley on the year.
  if (end_month > start_month) {
    max_firsts += (end_month - start_month);  // Accomodates for the extra
                                              // months if the ending month is
                                              // later than the starting month
    return max_firsts;
  } else {
    max_firsts -= start_month - end_month;  // Takes away extraneous months if
                                            // the ending month is earlier than
                                            // the starting month.
    return max_firsts;
  }
}

int number_of_first_of_months(
    string earlier, string later) {  // Returns the number of firsts of months
  int start_year, start_month, start_day;
  string_date_to_ints(earlier, start_year, start_month, start_day);
  int end_year, end_month, end_day;
  string_date_to_ints(later, end_year, end_month, end_day);
  return (firsts_calculation(start_year, start_month, start_day, end_year,
                             end_month, end_day));
}

double interest_earned(double begin_bal, double apr, string begin_date,
                       string end_date) {  // Calculates total interest earned
                                           // between two dates based on balance
                                           // and apr
  double interest = 0;
  double single_interest = 0;  // a single accruement of interest
  int capitalizations = number_of_first_of_months(begin_date, end_date);
  if (begin_bal <= 0) {
    return 0;
  }
  for (int i = 0; i < capitalizations; i++) {
    single_interest = interest_for_month(begin_bal, apr);
    begin_bal += single_interest;
    interest += single_interest;
  }
  return interest;
}

void ostring_bldr(std::istringstream &input_line, std::ostringstream &out_str,
                  string &command, string &date,
                  double &amount) {  // Parses through the istringstream to
                                     // store the action and amount as
                                     // variables, and build the ostringstream
  string val, prev_date;
  int count = 0;
  while (input_line >> val) {
    if (count == 0) {
      date = val;
      out_str << "On " << val << ":"
              << " Instructed to perform \"";
      count++;
    } else if (count == 1) {
      command = val;
      out_str << val << " ";
      count++;
    } else if (count == 2) {
      amount = stod(val.substr(1));
      out_str << val << "\"";
    }
  }
}

void cash_to_dollars_and_cents(double bal, int &dol,
                               int &cent) {  // Converts the balance into
                                             // dollars and cents for easy
                                             // outputting to stream
  dol = (int)bal;
  double cent_deci = abs(((bal - dol) * 1000));
  cent_deci = cent_deci / 10;
  cent = ceil(cent_deci);
}

string process_command(
    string line, string &prev_date, double &prev_bal,
    double &apr) {  // Fulfills the command set out by the string line
  string val, action, date;
  std::istringstream input(line);
  std::ostringstream oss;
  double amount;
  ostring_bldr(input, oss, action, date, amount);
  if (action == "Deposit") {
    if (prev_date != "" && number_of_first_of_months(date, prev_date) != 0) {
      int interest_count = number_of_first_of_months(prev_date, date);
      double total_interest = interest_earned(prev_bal, apr, prev_date, date);
      deposit(prev_bal, amount);
      prev_bal += total_interest;
      oss << "\nSince " << prev_date << ", interest has accrued "
          << interest_count << " times.\n"
          << "$";
      oss << std::setprecision(2) << std::fixed << total_interest
          << " interest has been earned.";
    } else {
      deposit(prev_bal, amount);
    }
  } else if (action == "Withdraw") {
    double copy_bal = prev_bal;  // Storing a copy of balance to check withdraw
    bool suc = withdraw(copy_bal, amount);
    if (suc == false) {
      int interest_count = number_of_first_of_months(prev_date, date);
      double total_interest = interest_earned(prev_bal, apr, prev_date, date);
      prev_bal += total_interest;
      if (prev_date != "" && number_of_first_of_months(date, prev_date) != 0) {
        oss << "\nSince " << prev_date << ", interest has accrued "
            << interest_count << " times.\n"
            << "$";
        oss << std::setprecision(2) << std::fixed << total_interest
            << " interest has been earned.";
      }
      oss << "\nOverdraft!";
      overdraft(prev_bal, amount);
    } else {
      if (prev_date != "" && number_of_first_of_months(date, prev_date) != 0) {
        int interest_count = number_of_first_of_months(prev_date, date);
        double total_interest = interest_earned(prev_bal, apr, prev_date, date);
        withdraw(prev_bal, amount);
        prev_bal += total_interest;
        oss << "\nSince " << prev_date << ", interest has accrued "
            << interest_count << " times.\n"
            << "$";
        oss << std::setprecision(2) << std::fixed << total_interest
            << " interest has been earned.";
      } else {
        withdraw(prev_bal, amount);
      }
    }
  }
  prev_date = date;
  int dollars = 0, cents = 0;
  cash_to_dollars_and_cents(prev_bal, dollars, cents);
  oss << "\nBalance: " << dollars << "." << std::right << std::setfill('0')
      << std::setw(2) << cents << "\n";
  string my_string = oss.str();
  std::cout << my_string;
  return my_string;
}

string process_commands(string process,
                        double apr) {  // Builds main istringstream and iterates
                                       // through it, building the main
                                       // ostringstream.
  string piece;
  std::ostringstream oss;
  std::istringstream process_stream(process);
  string pre_date = "";
  double balance = 0.0;
  while (std::getline(process_stream, piece)) {
    oss << process_command(piece, pre_date, balance, apr);
  }
  return oss.str();
}

int main() {}


