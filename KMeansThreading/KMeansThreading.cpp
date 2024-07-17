
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdlib> // for exit function
#include <vector>
#include <cmath>
#include <ctime>
#include <thread>
#include <algorithm>
#include <mutex>

using namespace std;


mutex mtx;

double distance(const vector<double>& p1, const vector<double>& p2) {
    double sum = 0.0;
    for (size_t i = 0; i < p1.size(); ++i) {
        double diff = p1[i] - p2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}


// Implementing E step
void assign_clusters(const vector<vector<double>>& X, vector<vector<vector<double>>>& clusters) {
    // Sequential

    int k = clusters.size();

    for (size_t idx = 0; idx < X.size(); ++idx) {
        vector<double> dist;
        const vector<double>& curr_x = X[idx];


        for (int i = 0; i < k; ++i) {
            dist.push_back(distance(curr_x, clusters[i][0]));
        }

        int curr_cluster = min_element(dist.begin(), dist.end()) - dist.begin();
        clusters[curr_cluster].push_back(curr_x);
    }
}


void assign_clusters_threaded(const vector<vector<double>>& X, vector<vector<vector<double>>>& clusters) {
    // With threads

    int k = clusters.size();
    int numthreads = thread::hardware_concurrency();
    int rows = X.size() / numthreads;
    int extra = X.size() % numthreads;
    int start = 0;    // row dimensions for first thread:
    int end = rows;

    vector<thread> workers3;

    vector<vector<double>> clusters_cpy(k);

    for (int i = 0; i < k; i++) {
        clusters_cpy[i] = clusters[i][0];
    }

    for (int t = 1; t <= numthreads; t++)
    {
        if (t == numthreads) { // last thread processes any extra rows:
            end += extra;
        }

        workers3.push_back(thread([&X, &clusters, &clusters_cpy](int start, int end, int t)
            {

                int k = clusters.size();
                for (size_t idx = start; idx < end; ++idx) {
                    vector<double> dist;
                    const vector<double>& curr_x = X[idx];


                    for (int i = 0; i < k; ++i) {
                        const vector<double>& curr_cluster = clusters_cpy[i];
                        dist.push_back(distance(curr_x, curr_cluster));
                    }

                    int curr_cluster = min_element(dist.begin(), dist.end()) - dist.begin();
                    {
                        lock_guard<mutex> lock(mtx); // Use lock_guard for automatic unlocking
                        clusters[curr_cluster].push_back(curr_x);
                    }
                }
            }, start, end, t)
        );
        start = end;
        end = start + rows;
    }//for

    // wait for threads to finish:
    for_each(workers3.begin(), workers3.end(), [](thread& t)
        {
            t.join();
        }
    );
}

// Implementing the M-Step
void update_clusters(vector<vector<vector<double>>>& clusters) {

    for (size_t i = 0; i < clusters.size(); ++i) {
        vector<vector<double>>& points = clusters[i];

        if (!points.empty()) {
            vector<double> new_center(points[0].size(), 0.0);

            for (const vector<double>& point : points) {
                for (size_t j = 0; j < point.size(); ++j) {
                    new_center[j] += point[j];
                }
            }

            for (size_t j = 0; j < new_center.size(); ++j) {
                new_center[j] /= points.size();
            }

            clusters[i].clear();
            clusters[i].push_back(new_center);
        }
    }
}

void update_clusters_threaded(vector<vector<vector<double>>>& clusters) {

    // With Threads

    vector<thread> workers3;

    for (int i = 0; i < clusters.size(); ++i)
    {


        workers3.push_back(thread([&clusters](int i)
            {

                vector<vector<double>>& points = clusters[i];

                if (!points.empty()) {
                    vector<double> new_center(points[0].size(), 0.0);

                    for (const vector<double>& point : points) {
                        for (size_t j = 0; j < point.size(); ++j) {
                            new_center[j] += point[j];
                        }
                    }

                    for (size_t j = 0; j < new_center.size(); ++j) {
                        new_center[j] /= points.size();
                    }
                    {
                        //lock_guard<mutex> lock(mtx);
                        clusters[i].clear();
                        clusters[i].push_back(new_center);
                    }

                }

            }, i)
        );
    }

    for_each(workers3.begin(), workers3.end(), [](thread& t)
        {
            t.join();
        }
    );
}



vector<int> pred_cluster(const vector<vector<double>>& X, const vector<vector<vector<double>>>& clusters) {
    int k = clusters.size();
    vector<int> pred;

    for (size_t i = 0; i < X.size(); ++i) {
        vector<double> dist;
        const vector<double>& curr_x = X[i];

        for (int j = 0; j < k; ++j) {
            dist.push_back(distance(curr_x, clusters[j][0]));
        }

        int predicted_cluster = min_element(dist.begin(), dist.end()) - dist.begin();
        pred.push_back(predicted_cluster);
    }

    return pred;
}

using namespace std::chrono;
int main()
{



    time_point<high_resolution_clock> startingTime;
    time_point<high_resolution_clock> endingTime;
    duration<double> deltaTime;
    unsigned long long n;
    const unsigned int numOfLoopFragments = 6;
    constexpr unsigned int numOfLoopFragmentsDim = numOfLoopFragments + 1;
    const unsigned int MaxNumOfTests = 10;  //Cardinality of each set of upper-bounds (the set of n's)
    const unsigned int MaxNumOfSets = 7;    //This is number of sets of upper-bounds (the n's)
    const unsigned int MaxNumTrials = 5;    //How many times we want to test a particular n
    const unsigned int setOfUpperBound_Ns[MaxNumOfSets][MaxNumOfTests]  //Store all the set of sets of n's
    {  //Here are the set of sets of n's
        /*0*/ { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 },
        /*1*/{ 100, 125, 150, 175, 200, 225, 250, 275, 300, 325 },
        /*2*/{ 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 },
        /*3*/{ 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000 },
        /*4*/{ 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000 },
        /*5*/{ 100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000 },
        /*6*/{ 1000000, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000, 8000000, 9000000, 10000000 }
    };
    unsigned int setIndex;

    //This code tests one loop at a time. The user chooses the loop to test. Then, chooses the
    //set of Ns to use to show the asymptotic growth. Each N in the set of Ns are timed MaxNumTrails
    //a number of trials. These times will be averaged giving a timing value for each N.

    double results[numOfLoopFragmentsDim][MaxNumTrials][MaxNumOfTests] = { 0.0 }; //+1 for correspondence of text loop #
    unsigned int loopFragmentIndex = 0;  // loop number 0 default to exit immediately with no testing.
    unsigned int nextNindex = 0; // column index for table of times for current N of the set of Ns

    //display the sets from which to choose
    std::cout << "Sets of Ns from which to choose.\n";
    for (unsigned int i = 0; i < MaxNumOfSets; i++) {
        cout << endl;
        cout << "Set " << i << " values: ";
        for (int j = 0; j < MaxNumOfTests; j++)
            cout << setOfUpperBound_Ns[i][j] << " ";
        cout << endl;
    }


    //Now, time the chosen loop fragment and store in runtime file with suffix name of loop index

    // ofstream constructor opens a file
    char loopIndex_cstr[2];
    //_itoa_s(loopFragmentIndex, loopIndex_cstr, 10);____________________________________________________

    cout << "Enter the set of Ns to used for iterations: ";
    cin >> setIndex;
    setIndex = 3;
    // at this point all is well, file for storing the runtime results is open and ready for receiving run times of loops

    //first loop from Weiss textbook Problem 2.7
    cout << "Timing loop (" << loopFragmentIndex << ")" << endl;
    for (int trialNum = 0; trialNum < MaxNumTrials; trialNum++) {
        cout << "Trial Number: " << trialNum << endl;
        for (nextNindex = 0; nextNindex < MaxNumOfTests; nextNindex++) {
            n = setOfUpperBound_Ns[setIndex][nextNindex];
            cout << "Input size N = " << n << endl;

            //time loop
            startingTime = high_resolution_clock::now();
            //Put code to be tested here and functions above----------

            //srand(time(NULL));

            srand(0);




            int n_samples = n;
            int n_features = 3;
            int centers = 100;
            int k = 5;

            vector<vector<double>> X(n_samples, vector<double>(n_features));

            for (int i = 0; i < n_samples; ++i) {
                for (int j = 0; j < n_features; ++j) {
                    X[i][j] = static_cast<double>(rand()) / RAND_MAX;
                }
            }


            // code ends here----------------
            vector<vector<vector<double>>> clusters(k);

            for (int idx = 0; idx < k; ++idx) {
                vector<double> center(n_features);
                for (int j = 0; j < n_features; ++j) {
                    center[j] = 2 * (2 * static_cast<double>(rand()) / RAND_MAX - 1);
                }
                clusters[idx].push_back(center);
            }

            vector<int> i(k);
            for (int j = 0; j < k; j++) {
                i[j] = j;
            }

            int itr = 15;
            for (int i = 0; i < itr; i++) {
                //Use non threaded functions
                //assign_clusters(X, clusters);
                //update_clusters(clusters);

                //Use threaded functions
                assign_clusters_threaded(X, clusters);
                update_clusters_threaded(clusters);

            }

            vector<int> pred = pred_cluster(X, clusters);
            // for(int n: pred)
            //     cout << n << ", ";
            endingTime = high_resolution_clock::now();
            deltaTime = duration_cast<duration<double>>(endingTime - startingTime);
            //finished timing, now store duration.

            results[loopFragmentIndex][trialNum][nextNindex] = deltaTime.count();
        }
    }

    //Print timing results for the chosen loop fragment
    cout << endl;
    cout << "Values of Ns and times T(N) for set " << setIndex << ":\n";
    //print "header": the values for Ns from chosen set of Ns.
    cout << "Trial";
    for (nextNindex = 0; nextNindex < MaxNumOfTests; nextNindex++) {
        cout << '\t' << setw(8) << setOfUpperBound_Ns[setIndex][nextNindex];   //don't worry about exact alignment
    }
    //print the times for chosen loopFragmentIndex
    cout << endl;
    cout << setprecision(7);
    for (unsigned int trialNum = 0; trialNum < MaxNumTrials; trialNum++) {
        cout << trialNum;
        for (nextNindex = 0; nextNindex < MaxNumOfTests; nextNindex++) {
            cout << '\t' << fixed << results[loopFragmentIndex][trialNum][nextNindex];
        }
        cout << endl;
    }


    return 0;
}







