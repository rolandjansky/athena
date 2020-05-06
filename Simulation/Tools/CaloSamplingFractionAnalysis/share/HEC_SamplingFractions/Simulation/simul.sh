#
Sim_tf.py --inputEVNTFile evgen.pool.root --outputHITSFile myHITS.pool.root --conditionsTag "default:OFLCOND-RUN12-SDR-31" --physicsList PhysicsList --randomSeed "default:5432100" --maxEvents 200 --skipEvents 0 --preInclude "EVNTtoHITS:preInclude.VertexPosOff.py,SimulationJobOptions/preInclude.CalHits.py,SimulationJobOptions/preInclude.ParticleID.py" --geometryVersion "default:ATLAS-R2-2016-00-00-00_VALIDATION"
#
