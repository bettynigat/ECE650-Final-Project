# ECE650 : Final Project Instructions

For the project you will need to:
*Augment your code from Assignment 4 in the way that is described below.
*Quantitatively analyze your software for various kinds of inputs
<br />
you should augment your code from Assignment 4 in the following ways.
*Make it multithreaded. You should have at least 7 threads: one for I/O, and one each for the
different approaches to solve the minimum vertex cover problem. You may find the examples
on https://git.uwaterloo.ca/ece650-1231/concurrency-demo useful.
*Implement the following five additional ways to solve MIN-VERTEX-COVER, in addition
to the REDUCTION-TO-CNF-SAT approach you had in Assignment 4. (We will call your
approach from Assignment 4, CNF-SAT-VC.)
<br />
1. Change the CNF-SAT-VC to a 3-CNF problem, where each clause has a maximum of
3 terms. We provided an approach in the slides on how to do that. We will call this
algorithm CNF-3-SAT-VC.
2. Pick a vertex of highest degree (most incident edges). Add it to your vertex cover and
throw away all edges incident on that vertex. Repeat till no edges remain. We will call
this algorithm APPROX-VC-1.
3. A refinement to the above algorithm. After you compute the set of vertices, go through
them and throw away any vertex you do not need. That is, if C is the vertex cover you
compute from running APPROX-VC-1, you can throw away vertex v from V if C − fvg
is a vertex cover. You can choose the vertices greedily. That is, simply go through the set
of vertices and throw the unnecessary ones away one-by-one. We will call this algorithm
REFINED-APPROX-VC-1.
4. Pick an edge hu; vi, and add both u and v to your vertex cover. Throw away all edges
attached to u and v. Repeat till no edges remain. We will call this algorithm APPROXVC-2.
15. A refinement to APPROX-VC-2, which we will call REFINED-APPROX-VC-2. In the
refinement, you take the output vertex cover of APPROX-VC-2 and throw away any
vertices that are not needed. As for APPROX-VC-1, you can choose the vertices greedily.
##Inputs
<br />
<br />
As input, use the output of /home/rbabaeec/ece650/graphGen/graphGen on ecetesla1. That
program generates graphs with the same number of edges for a particular number of vertices, but
not necessarily the same edges. Note that you can store its output in a file and use the file on other
machines.
<br />
##Output
Given a graph as input, your program should output the vertex cover computed by each approach
in sorted order. That is, give the following input:
V 5
E {<2,1>,<2,0>,<2,3>,<1,4>,<4,3>}
The output from your program should be:
CNF-SAT-VC: 2,4
CNF-3-SAT-VC: 2,4
APPROX-VC-1: 2,4
APPROX-VC-2: 0,2,3,4
REFINED-APPROX-VC-1: 2,4
REFINED-APPROX-VC-2: 2,4
<br />
That is, the name of the algorithm, followed by a colon ’:’, a single space, and then the computed
result as a sorted sequence of vertices, separated by commas.
<br />
##Analysis
You should analyze how efficient each approach is, for various inputs. An input is characterized
by the number of vertices. \Efficient" is characterized in one of two ways: (1) running time, and
(2) approximation ratio. We characterize the approximation ratio as the ratio of the size of the
computed vertex cover to the size of an optimal (minimum-sized) vertex cover.
For measuring the running time, use pthread getcpuclockid(). For an example of how it is
used, see http://www.kernel.org/doc/man-pages/online/pages/man3/pthread_getcpuclockid.
3.html.
<br />
For measuring the approximation ratio, compare it to the output of CNF-SAT-VC, which is
guaranteed to be optimal. CNF-3-SAT-VC will be also optimal
Your objective is to measure, for various values of jV j (number of vertices), for the graphs
generated by graphGen, the running time and approximation ratio. You should do this by generating
graphs for jV j 2 [5; 50] using that program, in increments of 5. That is, graphs with 5; 10; 15; : : : ; 50
vertices.
<br />
You should generate at least 10 graphs for each value for jV j, compute the time and approximation ratio for each such graph. You should measure the running time for at least 10 runs of each
such graph. Then, you should compute the mean (average) and standard deviation across those
100 runs for each value of jV j. For the approximation ratio, if there is any random component (e.g.,
which edges you choose, for APPROX-VC-2), then you should measure that multiple times as well
for each graph.
<br />
2You might find the optimal approach (CNF-SAT-VC) difficult to scale to large number of vertices. For these instances, you can use a timeout to avoid waiting for its result and produce
CNF-SAT-VC: timeout in the output.
<br />
This will be the case for CNF-3-SAT-VC as well; however, the question you will need to answer
in your analysis is that if changing the encoding to 3-CNF impacted the performance or not.
CNF-SAT-VC can be scaled significantly by improving the encoding. Should you decide to
improve the encoding, bonus points will be given for scaling to larger instances. Make sure to
describe the improvements you made to the encoding in the report. Points will be deducted for
encodings that are scalable but are either incorrect or unexplaine
