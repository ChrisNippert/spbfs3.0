# Run 3


# Notes
With K-fill, k guesses are better instead of just the "most recent" publishes
but at large k counts, it can slow down due to thread cache invalidation.
At longer expansion times it does perform better since it's better at predicting expansions

# Run 2

Using top K-fill into the bench instead of circular-queue style insertion

Expansions per second (mean of runs), columns = added expansion cost

                    0µs   1µs   2µs   5µs  10µs  20µs
config                                               
spastar t=1 k=16   420k  285k  233k  136k   80k   44k
spastar t=1 k=32   406k  261k  232k  135k   80k   44k
spastar t=1 k=64   404k  285k  214k  132k   79k   44k
spastar t=1 k=128  403k  259k  215k  128k   78k   44k
spastar t=2 k=16   533k  366k  352k  221k  142k   81k
spastar t=2 k=32   509k  389k  355k  225k  143k   82k
spastar t=2 k=64   545k  381k  356k  227k  146k   82k
spastar t=2 k=128  447k  392k  355k  225k  141k   82k
spastar t=4 k=16   504k  495k  492k  341k  216k  135k
spastar t=4 k=32   462k  483k  513k  360k  238k  143k
spastar t=4 k=64   381k  419k  500k  367k  247k  147k
spastar t=4 k=128  265k  312k  466k  358k  247k  150k
spastar t=8 k=16   368k  391k  389k  374k  285k  184k
spastar t=8 k=32   329k  348k  352k  379k  310k  207k
spastar t=8 k=64   252k  248k  248k  251k  295k  220k
spastar t=8 k=128  202k  194k  197k  190k  267k  223k


# Run 1
Using circular-queue bench insertion

Expansions per second (mean of runs), columns = added expansion cost

                    0µs   1µs   2µs   5µs  10µs  20µs
config                                               
astar              524k  358k  258k  139k   81k   45k
pastar  t=1        424k  284k  228k  134k   80k   44k
pastar  t=2        483k  422k  386k  237k  154k   86k
pastar  t=4        556k  535k  506k  405k  284k  167k
pastar  t=8        578k  561k  562k  515k  459k  302k

                    0µs   1µs   2µs   5µs  10µs  20µs
config                                               
spastar t=1 k=16   448k  290k  229k  135k   79k   44k
spastar t=1 k=32   448k  294k  230k  135k   81k   44k
spastar t=1 k=64   421k  280k  236k  136k   80k   44k
spastar t=1 k=128  446k  296k  229k  132k   80k   44k
spastar t=2 k=16   646k  466k  362k  236k  145k   83k
spastar t=2 k=32   660k  468k  386k  239k  145k   84k
spastar t=2 k=64   615k  473k  380k  240k  147k   83k
spastar t=2 k=128  600k  478k  378k  240k  145k   81k
spastar t=4 k=16   697k  671k  549k  345k  216k  123k
spastar t=4 k=32   716k  678k  548k  357k  222k  125k
spastar t=4 k=64   713k  686k  571k  362k  225k  129k
spastar t=4 k=128  712k  704k  582k  371k  238k  140k
spastar t=8 k=16   638k  664k  626k  478k  279k  157k
spastar t=8 k=32   640k  667k  513k  485k  289k  165k
spastar t=8 k=64   675k  666k  620k  509k  318k  183k
spastar t=8 k=128  683k  633k  610k  504k  344k  199k