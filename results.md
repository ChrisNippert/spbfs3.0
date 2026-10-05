# Run 8 (Moved heuristic calculation inside the speculation of spastar)


# Run 7 (Run on ./maps/path_data/100_100_0.4_50_5)
base expansion ≈ 0.53 µs
Expansions per second (mean of runs), columns = added expansion cost

                      0µs     1µs   2µs   5µs  10µs  20µs
config                                                   
astar              1,874k    613k  376k  174k   93k   48k
pastar  t=1        1,586k    591k  367k  173k   92k   48k
pastar  t=2        1,054k    817k  614k  331k  181k   95k
pastar  t=4          966k    798k  751k  583k  350k  188k
pastar  t=8          918k    846k  795k  768k  608k  359k
spastar t=1 k=16   1,763k    611k  376k  175k   93k   48k
spastar t=1 k=32   1,767k    606k  372k  175k   93k   48k
spastar t=1 k=64   1,712k    599k  369k  173k   92k   48k
spastar t=1 k=128  1,585k    588k  367k  173k   92k   48k
spastar t=2 k=16   1,758k  1,006k  652k  313k  168k   87k
spastar t=2 k=32   1,809k  1,045k  683k  329k  177k   92k
spastar t=2 k=64   1,732k  1,067k  694k  336k  181k   94k
spastar t=2 k=128  1,600k  1,050k  690k  335k  181k   94k
spastar t=4 k=16   1,588k  1,196k  868k  463k  258k  137k
spastar t=4 k=32   1,705k  1,273k  925k  505k  282k  150k
spastar t=4 k=64   1,686k  1,266k  980k  543k  306k  163k
spastar t=4 k=128  1,569k  1,217k  976k  566k  325k  174k
spastar t=8 k=16   1,058k    964k  778k  464k  277k  153k
spastar t=8 k=32   1,320k  1,077k  871k  530k  316k  176k
spastar t=8 k=64   1,435k  1,137k  929k  575k  351k  195k
spastar t=8 k=128  1,478k  1,116k  929k  599k  371k  210k

# Run 6 (Other Optimizations)
Only deal with clock garbage if extra time > 0
Proxy euclidean distance that acts the same but is faster.
Reserve best_g map on startup, removing rehashing
Made spbfs spinning speculators pause if they failed to claim something on a run

base expansion ≈ 0.50 µs
Expansions per second (mean of runs), columns = added expansion cost

                      0µs     1µs     2µs   5µs  10µs  20µs
config                                                     
astar              2,019k    659k    398k  181k   95k   49k
pastar  t=1        1,707k    606k    376k  177k   94k   48k
pastar  t=2        1,215k    928k    661k  340k  184k   96k
pastar  t=4        1,132k    934k    888k  619k  353k  188k
pastar  t=8        1,072k    991k    916k  895k  633k  363k
spastar t=1 k=16   2,497k    680k    401k  182k   95k   49k
spastar t=1 k=32   2,321k    682k    401k  182k   95k   49k
spastar t=1 k=64   2,300k    670k    392k  180k   95k   49k
spastar t=1 k=128  2,130k    659k    384k  179k   94k   49k
spastar t=2 k=16   2,289k  1,144k    722k  336k  178k   91k
spastar t=2 k=32   2,285k  1,166k    722k  341k  182k   94k
spastar t=2 k=64   2,334k  1,170k    713k  344k  182k   94k
spastar t=2 k=128  1,956k  1,127k    719k  341k  181k   94k
spastar t=4 k=16   1,862k  1,663k  1,156k  567k  307k  160k
spastar t=4 k=32   1,925k  1,742k  1,210k  599k  328k  170k
spastar t=4 k=64   2,072k  1,830k  1,250k  634k  342k  179k
spastar t=4 k=128  1,869k  1,775k  1,271k  642k  348k  182k
spastar t=8 k=16   1,330k  1,413k  1,200k  753k  441k  242k
spastar t=8 k=32   1,386k  1,552k  1,482k  887k  513k  277k
spastar t=8 k=64   1,592k  1,708k  1,567k  950k  556k  299k
spastar t=8 k=128  1,458k  1,557k  1,492k  978k  583k  316k


# Run 5 (State Size Optimization)
"Oh no thats fast"

Representation changes beat parallelization: a 16-byte state made serial A* 4× faster,
removed spastar's single-thread overhead entirely,
and doubled spastar's advantage over lock-based parallel A*

base expansion ≈ 0.49 µs
Expansions per second (mean of runs), columns = added expansion cost

                      0µs     1µs     2µs     5µs  10µs  20µs
config                                                       
astar              2,069k    643k    394k    178k   92k   48k
pastar  t=1        1,480k    602k    380k    175k   93k   48k
pastar  t=2        1,111k    956k    678k    333k  180k   95k
pastar  t=4        1,134k    996k    928k    616k  345k  188k
pastar  t=8        1,072k    999k    965k    873k  605k  365k
spastar t=1 k=16   2,276k    693k    398k    181k   94k   48k
spastar t=1 k=32   2,080k    683k    390k    181k   93k   48k
spastar t=1 k=64   2,134k    680k    375k    181k   93k   48k
spastar t=1 k=128  1,848k    651k    390k    179k   94k   48k
spastar t=2 k=16   2,402k  1,156k    719k    332k  178k   91k
spastar t=2 k=32   2,588k  1,218k    722k    336k  181k   94k
spastar t=2 k=64   2,175k  1,194k    740k    336k  182k   94k
spastar t=2 k=128  2,108k  1,115k    747k    338k  181k   95k
spastar t=4 k=16   2,038k  1,701k  1,112k    574k  308k  160k
spastar t=4 k=32   2,142k  1,840k  1,249k    606k  326k  172k
spastar t=4 k=64   2,174k  1,847k  1,302k    624k  344k  180k
spastar t=4 k=128  2,082k  1,887k  1,314k    637k  352k  184k
spastar t=8 k=16   1,276k  1,360k  1,259k    759k  446k  242k
spastar t=8 k=32   1,526k  1,600k  1,460k    902k  521k  278k
spastar t=8 k=64   1,654k  1,698k  1,524k    983k  563k  300k
spastar t=8 k=128  1,640k  1,696k  1,576k  1,014k  596k  319k

# Run 4 (alignas(64) for SNode and Async Stats)
Results show no large difference, probably just noise

Expansions per second (mean of runs), columns = added expansion cost

                    0µs   1µs   2µs   5µs  10µs  20µs
config
spastar t=1 k=16   396k  281k  218k  131k   80k   44k
spastar t=1 k=32   385k  288k  218k  133k   79k   44k
spastar t=1 k=64   380k  277k  217k  133k   79k   44k
spastar t=1 k=128  366k  269k  216k  130k   79k   43k
spastar t=2 k=16   501k  413k  331k  222k  139k   80k
spastar t=2 k=32   497k  427k  346k  218k  140k   81k
spastar t=2 k=64   529k  416k  335k  227k  143k   82k
spastar t=2 k=128  512k  409k  345k  223k  144k   82k
spastar t=4 k=16   563k  527k  475k  339k  223k  135k
spastar t=4 k=32   582k  550k  496k  356k  236k  142k
spastar t=4 k=64   572k  561k  496k  360k  246k  147k
spastar t=4 k=128  602k  562k  494k  368k  248k  151k
spastar t=8 k=16   514k  516k  478k  391k  290k  189k
spastar t=8 k=32   520k  538k  506k  430k  328k  213k
spastar t=8 k=64   538k  522k  509k  407k  339k  227k
spastar t=8 k=128  531k  514k  507k  480k  354k  236k


# Run 3 (Bench Smart-Fill)
Smart K-fill, only filling the parts in the bench that have changed

Smart fill fixes top-K's cache-invalidation cost but still pays O(K) scanning per expansion on the main thread, so circular remains faster at low cost; smart fill wins at high cost due to better prediction.

Expansions per second (mean of runs), columns = added expansion cost

                    0µs   1µs   2µs   5µs  10µs  20µs
config                                               
spastar t=1 k=16   383k  287k  222k  133k   80k   44k
spastar t=1 k=32   373k  300k  219k  130k   80k   44k
spastar t=1 k=64   412k  262k  212k  133k   79k   44k
spastar t=1 k=128  368k  262k  216k  132k   79k   44k
spastar t=2 k=16   498k  361k  339k  223k  139k   81k
spastar t=2 k=32   490k  367k  332k  225k  143k   82k
spastar t=2 k=64   481k  389k  348k  217k  143k   83k
spastar t=2 k=128  482k  371k  337k  226k  141k   82k
spastar t=4 k=16   563k  474k  454k  330k  225k  135k
spastar t=4 k=32   586k  490k  484k  353k  236k  142k
spastar t=4 k=64   560k  476k  478k  360k  242k  147k
spastar t=4 k=128  551k  504k  493k  364k  249k  151k
spastar t=8 k=16   493k  465k  448k  407k  294k  190k
spastar t=8 k=32   516k  528k  509k  443k  330k  215k
spastar t=8 k=64   462k  495k  515k  470k  348k  226k
spastar t=8 k=128  515k  496k  512k  472k  356k  238k

# Notes
With K-fill, k guesses are better instead of just the "most recent" publishes
but at large k counts, it can slow down due to thread cache invalidation.
At longer expansion times it does perform better since it's better at predicting expansions

# Run 2 (Bench K-Fill)
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


# Run 1 (re-measured after pastar/astar comparator fix)
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