#include "plagiarism_checker.hpp"

void plagiarism_checker_t::add_submission(std::shared_ptr<submission_t> __submission) {
    submissions[total_submissions] = (std::make_pair(__submission, std::chrono::system_clock::now() - start_time));
    total_submissions++;
}

void plagiarism_checker_t::hash_function() {
    bool finished=false;
    while (start_worker or !finished) {
        int count=0;
        for (int i = 0; i < total_submissions; i++) {
            if (!hashed[i] && !submissions[i].first->codefile.empty()) {

                tokenizer_t tokenizer(submissions[i].first->codefile);
                auto tokens = tokenizer.get_tokens();

                long long int hashval = 0;
                std::vector<long long int> hashed_tokens;
                long long int current_hash=1;
                for(int j=0;j<5;j++){
                    hashval=(hashval*BASE+tokens[j]);
                    current_hash=(current_hash*BASE);
                }
                hashval=hashval%current_hash;
                hashed_tokens.push_back(hashval);
                for (int j = 5; j < tokens.size(); j++) {
                    hashval = (hashval * BASE + tokens[j]) % current_hash;
                    hashed_tokens.push_back(hashval);
                }

                database[i] = hashed_tokens;
                hashed[i] = true;
            }
            if (hashed[i]) {
                count++;
            }
        }
        if(count == total_submissions){
            finished=true;
        }
    }
}

void plagiarism_checker_t::check_plagiarism(int submission1, int submission2) {

    std::vector<std::pair<int, int>> matches;
    int max_size = 0;
    std::vector<int> flagged_sub;

    if (std::chrono::duration_cast<std::chrono::seconds>(submissions[submission1].second - submissions[submission2].second).count() < 1) {
        flagged_sub.push_back(submission1);
        flagged_sub.push_back(submission2);
    }
    else {
        if (submissions[submission1].second <= submissions[submission2].second ) {
            flagged_sub.push_back(submission2);
        }
        else {
            flagged_sub.push_back(submission1);
        }
    }

    for (int i = 0; i < database[submission1].size() - 10; i++) {
        for (int j = 0; j < database[submission2].size() - 10; j++) {
            if (database[submission1][i] == database[submission2][j] && database[submission1][i + 10] == database[submission2][j + 10] && database[submission1][i + 5] == database[submission2][j + 5]) {
                for (int k = 0; k < flagged_sub.size(); k++) {
                    if (flagged_sub[k] == submission1) {
                        total_matches[submission2].push_back(j);
                    } else {
                        total_matches[submission1].push_back(i);
                    }
                }
                matches.push_back(std::make_pair(i, j));
                if (max_size < MIN_EXACT_MATCH_LENGTH) {
                    int size = check_plagiarism_helper(submission1, submission2, i, j);
                    if (size > max_size) {
                        max_size = size;
                    }
                }
            }
        }
    }

    bool found = false;
    std::sort(matches.begin(), matches.end());
    std::vector<std::pair<int, int>> twin_matches;

    for (int k = 0; k < matches.size(); k++) {
        if (k == 0) {
            twin_matches.push_back(matches[k]);
        }
        else {
            if (matches[k].first - twin_matches[twin_matches.size() - 1].first >= 15) {
                twin_matches.push_back(matches[k]);
            }
        }
    }

    if (max_size >= MIN_EXACT_MATCH_LENGTH) {
        found = true;
    }

    if (twin_matches.size() >= MIN_EXACT_MATCHES) {
        found = true;
    }

    if (found) {
        for (int k = 0; k < flagged_sub.size(); k++) {
            if (plagged[flagged_sub[k]]) {
                continue;
            }
            if(flagged_sub[k]>=nOriginal){
                plagged[flagged_sub[k]] = true;
                if (submissions[flagged_sub[k]].first->student != nullptr) {
                    submissions[flagged_sub[k]].first->student->flag_student(submissions[flagged_sub[k]].first);
                }
                if (submissions[flagged_sub[k]].first->professor != nullptr) {
                    submissions[flagged_sub[k]].first->professor->flag_professor(submissions[flagged_sub[k]].first);
                }
            }
        }
    }

    std::vector<int> patchwork_checking = {submission1, submission2};
    for (int l = 0; l < patchwork_checking.size(); l++) {
        if (plagged[patchwork_checking[l]]) {
            continue;
        }
        else{
            std::sort(total_matches[patchwork_checking[l]].begin(), total_matches[patchwork_checking[l]].end());
            int a = 1;
            while (a < total_matches[patchwork_checking[l]].size() && a < 21) {
                if (total_matches[patchwork_checking[l]][a] - total_matches[patchwork_checking[l]][a - 1] < 15) {
                    total_matches[patchwork_checking[l]].erase(total_matches[patchwork_checking[l]].begin() + a);
                }
                else {
                    a++;
                }
            }
            if (total_matches[patchwork_checking[l]].size() >= PATCHWORK_THRESHOLD) {
                if(patchwork_checking[l]>=nOriginal){
                    plagged[patchwork_checking[l]] = true;
                    if (submissions[patchwork_checking[l]].first->student != nullptr) {
                        submissions[patchwork_checking[l]].first->student->flag_student(submissions[patchwork_checking[l]].first);
                    }
                    if (submissions[patchwork_checking[l]].first->professor != nullptr) {
                        submissions[patchwork_checking[l]].first->professor->flag_professor(submissions[patchwork_checking[l]].first);
                    }
                }
            }
        }
    }
}

int plagiarism_checker_t::check_plagiarism_helper(int submission1, int submission2, int i, int j) {
    int size = 0;
    while (i < database[submission1].size() && j  < database[submission2].size()) {
        if (database[submission1][i] == database[submission2][j]) {
            size += 5;
            i += 5;
            j += 5;
        } else {
            break;
        }
    }
    return size;
}

void plagiarism_checker_t::main_function() {
    bool finished = false;
    while (start_worker or !finished){
        int count = 0;
        for (int i = 0; i < total_submissions; i++) {
            for (int j = 0; j < i; j++) {
                if (!checkedMatrix[i][j] && hashed[i] && hashed[j]) {
                    checkedMatrix[i][j] = 1;
                    checkedMatrix[j][i] = 1;
                    check_plagiarism(i, j);
                }
                if (checkedMatrix[i][j]==1) {
                    count++;
                }
            }
        }
        if (count == total_submissions * (total_submissions - 1) / 2 && !start_worker) {
            finished = true;
        }
    }
}

plagiarism_checker_t::plagiarism_checker_t() : start_worker(true) {
    nOriginal = 0;
    total_submissions = 0;
    submissions.resize(MAX_SIZE);
    database.resize(MAX_SIZE);
    total_matches.resize(MAX_SIZE);
    hashed.resize(MAX_SIZE, false);
    plagged.resize(MAX_SIZE, false);
    checkedMatrix.resize(MAX_SIZE);
    for (int i = 0; i < MAX_SIZE; ++i) {
        checkedMatrix[i].resize(MAX_SIZE, 0);
        checkedMatrix[i][i] = 1;
    }
    dThreads.push_back(std::thread(&plagiarism_checker_t::hash_function, this));
    dThreads.push_back(std::thread(&plagiarism_checker_t::main_function, this));
}

plagiarism_checker_t::plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions) : start_worker(true) {
    submissions.resize(MAX_SIZE);
    total_submissions = __submissions.size();
    database.resize(MAX_SIZE);
    total_matches.resize(MAX_SIZE);
    hashed.resize(MAX_SIZE, false);
    plagged.resize(MAX_SIZE, false);
    nOriginal=__submissions.size();
    checkedMatrix.resize(MAX_SIZE);
    for (int i = 0; i < MAX_SIZE; ++i) {
        if (i < __submissions.size()) {
            checkedMatrix[i].resize(MAX_SIZE, 1);
        } else {
            checkedMatrix[i].resize(MAX_SIZE, 0);
            checkedMatrix[i][i] = 1;
        }
    }
    for (int i = 0; i < __submissions.size(); i++) {
        auto submission = __submissions[i];
        submissions[i] = std::make_pair(submission, std::chrono::seconds(0));
    }
    dThreads.push_back(std::thread(&plagiarism_checker_t::hash_function, this));
    dThreads.push_back(std::thread(&plagiarism_checker_t::main_function, this));
}

plagiarism_checker_t::~plagiarism_checker_t() {
    start_worker = false;
    for (auto& thread : dThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}