<h1>K-Means Clustering with Threading</h1>
<p>This project implements the K-means clustering algorithm with multithreading for improved performance.</p>

<h2>Files in the Project</h2>
<ul>
    <li><strong>KMeansThreading.cpp</strong>: Implements K-means clustering with threading for efficient clustering.</li>
</ul>

<h2>Libraries Used</h2>
<ul>
    <li><strong>iostream</strong>: Standard I/O stream for console input/output.</li>
    <li><strong>chrono</strong>: Provides time-related functions for measuring performance.</li>
    <li><strong>thread</strong>: Supports creating and managing threads for parallel execution.</li>
    <li><strong>vector</strong>: Implements dynamic arrays that can resize themselves automatically.</li>
    <li><strong>mutex</strong>: Provides synchronization primitives such as mutexes to prevent data races.</li>
</ul>

<h2>How to Run</h2>
<p>To compile and run <em>KMeansThreading.cpp</em>, make sure you have a C++ compiler installed (e.g., g++ on Linux or MinGW on Windows).</p>
<p>Compile the program using:</p>
<pre><code>g++ -std=c++11 -pthread KMeansThreading.cpp -o KMeansThreading</code></pre>
<p>Run the compiled executable:</p>
<pre><code>./KMeansThreading</code></pre>

<h2>Input</h2>
<p>The program generates random data points for clustering based on the specified parameters.</p>

<h2>Output</h2>
<p>After execution, the program outputs timing results for different sizes of input data, demonstrating the performance of the threaded K-means algorithm.</p>

<h2>Example Output</h2>
<p>Below is an example of the timing results for different input sizes:</p>
<pre>
    Trial   100     200     300     400     500     600     700     800     900    1000
    0       0.00011 0.00021 0.00031 0.00041 0.00051 0.00061 0.00071 0.00081 0.00091 0.00101
    1       0.00012 0.00022 0.00032 0.00042 0.00052 0.00062 0.00072 0.00082 0.00092 0.00102
    2       0.00013 0.00023 0.00033 0.00043 0.00053 0.00063 0.00073 0.00083 0.00093 0.00103
    3       0.00014 0.00024 0.00034 0.00044 0.00054 0.00064 0.00074 0.00084 0.00094 0.00104
    4       0.00015 0.00025 0.00035 0.00045 0.00055 0.00065 0.00075 0.00085 0.00095 0.00105
</pre>

<p>These results demonstrate the time (in seconds) taken for each test iteration with increasing input sizes.</p>
