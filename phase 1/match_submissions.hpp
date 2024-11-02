#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include <unordered_map>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here
const int SHORT_MIN_LEN = 10;
const int SHORT_MAX_LEN = 20;
const int LONG_MIN_LEN = 30;
const double LONG_MATCH_THRESHOLD = 0.8;
const int MOD = 1e9 + 7;
const int BASE = 257;

std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    std::unordered_map<int, std::pair<int, int>> short_hashes1; // hash -> (start_index, length)
    std::unordered_map<int, std::pair<int, int>> short_hashes2;
    
    int total_short_match_len = 0;
    int longest_match_len = 0;
    int longest_match_start1 = -1;
    int longest_match_start2 = -1;

    // Helper function to calculate rolling hash of a window
    auto compute_hash = [](const std::vector<int> &arr, int start, int len) {
        long long hash_value = 0;
        for (int i = 0; i < len; ++i) {
            hash_value = (hash_value * BASE + arr[start + i]) % MOD;
        }
        return static_cast<int>(hash_value);
    };

    // Step 1: Find short pattern matches of length between 10-20
    for (int len = SHORT_MIN_LEN; len <= SHORT_MAX_LEN; ++len) {
        int i = 0, j = 0; 

        while (i + len <= submission1.size()) {
            int hash_value = compute_hash(submission1, i, len);
            short_hashes1[hash_value] = {i, len};
            ++i;
        }

        // Search for matches in submission2 while advancing j to avoid overlaps
        while (j + len <= submission2.size()) {
            int hash_value = compute_hash(submission2, j, len);

            // Check if there's a match and if the lengths align
            if (short_hashes1.count(hash_value) && short_hashes1[hash_value].second == len) {
                int start1 = short_hashes1[hash_value].first;

                // Add the matched length to the total
                total_short_match_len += len;

                // Move the indices i and j to avoid overlapping this matched section
                i = start1 + len;  
                j += len;          

                short_hashes1.erase(hash_value);
                break;  // Exit the current length match search once a match is found
            }
            ++j;
        }
    }

    
    // Step 2: Find long approximate pattern matches of length 30+
    for (int len = LONG_MIN_LEN; len <= submission1.size(); ++len) {
        for (int i = 0; i + len <= submission1.size(); ++i) {
            int hash_value = compute_hash(submission1, i, len);
            short_hashes1[hash_value] = {i, len};
        }
        for (int j = 0; j + len <= submission2.size(); ++j) {
            int hash_value = compute_hash(submission2, j, len);
            if (short_hashes1.count(hash_value)) {
                int start1 = short_hashes1[hash_value].first;
                int common_len = 0;
                for (int k = 0; k < len; ++k) {
                    if (submission1[start1 + k] == submission2[j + k]) ++common_len;
                }
                if (common_len >= LONG_MATCH_THRESHOLD * len) {
                    if (common_len > longest_match_len) {
                        longest_match_len = common_len;
                        longest_match_start1 = start1;
                        longest_match_start2 = j;
                    }
                }
            }
        }
    }

    // Step 3: Populate results
    result[0] = (total_short_match_len > 100 || longest_match_len >= LONG_MIN_LEN) ? 1 : 0; // example threshold for plagiarism
    result[1] = total_short_match_len;
    result[2] = longest_match_len;
    result[3] = longest_match_start1;
    result[4] = longest_match_start2;
    //std::cerr<<result[0]<<" "<<result[1]<<" "<<result[2]<<" "<<result[3]<<" "<<result[4]<<std::endl;
    return result; // dummy return
    // End TODO
}