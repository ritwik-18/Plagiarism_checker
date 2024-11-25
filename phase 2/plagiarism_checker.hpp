#include "structures.hpp"
#include <vector>
#include <thread>
#include<chrono>
#include <algorithm>
#include <cmath>
#include <mutex>
#include<atomic>
#include<unordered_map>

// -----------------------------------------------------------------------------

#define MOD 1000000007
#define BASE 37
#define MAX_SIZE 1000
#define PATCHWORK_THRESHOLD 20
#define MIN_PATTERN_LENGTH 15
#define MIN_EXACT_MATCH_LENGTH 75
#define MIN_EXACT_MATCHES 10

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and classes here

class plagiarism_checker_t {
    // You should NOT modify the public interface of this class.
public:
    plagiarism_checker_t(void);
    plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> 
                            __submissions);
    ~plagiarism_checker_t(void);
    void add_submission(std::shared_ptr<submission_t> __submission);

protected:
    // TODO: Add members and function signatures here
    std::vector<std::pair<std::shared_ptr<submission_t>, std::chrono::system_clock::duration>> submissions;
    std::vector<std::vector<long long int>> database;
    std::vector<std::thread> dThreads;
    std::vector<std::vector<bool>>checkedMatrix;
    std::vector<std::vector<int>>total_matches;
    std::vector<bool>hashed;
    std::vector<bool>plagged;
    std::vector<int>get_tokens(const std::string& file_path);
    std::chrono::system_clock::time_point start_time;
    std::unordered_map<int,std::vector<std::pair<int,int>>>global_database;
    int nOriginal;
    int total_submissions;
    int database_pending;
    bool start_worker;
    void main_function();
    void global_comparator();
    void hash_function();
    void check_plagiarism(int submission_id1,int submission_id2);
    void Plagger(int submission_id);
    int check_plagiarism_helper(int submission_id1,int submissionid2, int i, int j);
    // End TODO
};