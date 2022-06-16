## Running Output Monitor Algorithm (HTTOutputMonitorAlg)

0. Clone and build the directory following `TrigHTT/README.md`

From the top-level working directory (`HTTSim/` if you followed the build instructions),

1. Source the build
    ```sh
    source build/x86_64-*-gcc62-opt/setup.sh
    ```
2. Create a work dir and a run dir
    ```sh
    mkdir workspace
    cd workspace
    mkdir rundir
    ```
3. Create an executable file `run`
    ```sh
    touch run
    chmod +x run
    ```
4. Copy the following contents into that file.
    ```sh
    #!/bin/bash

    HTTLogHitFile='/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/eta0103phi0305/References/loghits__21.9.15-ATLAS-P2-ITK-23-00-01__EF_TaskForce_dev23__EF_TaskForce_dev23__dev_21-02-15.root'

    cd rundir/
    HTTOutputMonitorAlg_tf.py \
        --maxEvents 5 \
        --InFileName $HTTLogHitFile \
    ```
5. Run the script. Note you must have eos mounted for this to work (i.e. lxplus)
    ```sh
    ./run
    ```

## Running Map Maker Algorithm (HTTMapMakerAlg)
The Map Maker algorithm will output a **plane map**, **region map**, and **subregion map**. It will also prodouce a **radii file** and **etapattern file**, which can be used with the bit shift implementation of the Hough Transform.

Do the same as above, but run a different transform. If you haven't already set up your working environment then:

0. Clone and build the directory following `TrigHTT/README.md`

From the top-level working directory (`HTTSim/` if you followed the build instructions),

1. Source the build
    ```sh
    source build/x86_64-*-gcc62-opt/setup.sh
    ```
2. Create a work dir and a run dir
    ```sh
    mkdir workspace
    cd workspace
    mkdir rundir
    ```
3. Create an executable file `run`
    ```sh
    touch run
    chmod +x run
    ```
4. Copy the following contents into that file.
```sh
#!/bin/bash

HTTInputRawHits='/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/eta0103phi0305/Wrappers/singlemu_invPtFlat1_1M_wrap.root'
OutputName='MMtest_'

cd rundir/

HTTMapMakerAlg_tf.py \
    --maxEvents 1000 \
    --trim 0.1 \
    --region 0 \
    --InFileName $HTTInputRawHits \
    --OutFileName $OutputName \
    --KeyString "strip,barrel,2" \
    --nSlices 10 \
```
5. Run the script. Note you must have eos mounted for this to work (i.e. lxplus)
    ```sh
    ./run
    ```

In this example ```trim 0.1``` will result in ignoring modules that are hit by less than 0.1 % of all tracks.

To run with 2D slicing, use the ```KeyString2``` argument to define the second key layer. Layers can be given as a physical layer (```"strip,barrel,2"```) or a logical layer (```"plane 0"```). Endcap Layers must be specified as positive or negative: ```"strip,posEndcap,2"```. 

Note that when using 2D slicing the MM will produce maps with a number of slices at the order of ```nSlices```<sup>2</sup>. And always ensure that the input HTTRawHit file is compatible with the region argument.
