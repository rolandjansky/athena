package atlantis.data;

import atlantis.event.AAssociation;
import atlantis.event.AEvent;
import atlantis.event.AData;
import atlantis.globals.AGlobals;
import atlantis.utils.AHashMap;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AIdHelper;
import atlantis.utils.ALogger;

/**
 *
 * @author Eric Jansen
 */

public class AInDetTrackData extends ATrackData {

   private  int[] author = null;

   protected int numBLayerHits[] = null;
   protected int numPixelHits[] = null;
   protected int numSCTHits[] = null;
   protected int numTRTHits[] = null;

   private static ALogger logger = ALogger.getLogger(ATrackData.class);

  /** Creates a new instance of AInDetTrackData */
  AInDetTrackData(AHashMap p, AEvent e) throws AAtlantisException {
    super(p,e);

    author = p.getUnsureIntArray("trackAuthor"); //Change added 17/02/2010

    numBLayerHits = p.getUnsureIntArray("nBLayerHits");
    numPixelHits = p.getUnsureIntArray("nPixHits");
    numSCTHits = p.getUnsureIntArray("nSCTHits");
    numTRTHits = p.getUnsureIntArray("nTRTHits");

    String assocKey = getFullName();
    if (p.get("numHits") != null) {
      int[] numHits = p.getIntArray("numHits");
      int[] hits = p.getIntArray("hits");

      // This was added into TrkJiveXML-00-01-24, July 2010. For backwards
      // compatibilty, we can also fill these arrays from the association
      if (numPixelHits == null && numSCTHits == null && numTRTHits == null) {
          numPixelHits = new int[numData];
          numSCTHits = new int[numData];
          numTRTHits = new int[numData];
          for (int i=0, num=0; i<numData; i++) {
              numPixelHits[i] = numSCTHits[i] = numTRTHits[i] = 0;

              for (int j=0; j<numHits[i]; j++, num++) {
                  try {
                      // Negative identifiers are pixels, the rest we can decode using AIdHelper
                      if (numHits[i] > 0 && (hits[num] < 0 || AIdHelper.subDetector(hits[num]) == 2)) {
                          switch(hits[num] < 0 ? 1 : AIdHelper.indetPart(hits[num])) {
                              case 1:
                                  numPixelHits[i]++;
                                  break;
                              case 2:
                                  numSCTHits[i]++;
                                  break;
                              case 3:
                                  numTRTHits[i]++;
                                  break;
                          }
                      }
                  } catch (AAtlantisException ex) {
                      logger.warn("Problem decoding hit identifier: " + ex.getMessage());
                  }
              }
          }
      }

      int[] driftSign = null;
      int[] isOutlier = null;
      if (p.get("driftSign") != null && p.get("isOutlier") != null) {
        driftSign = p.getIntArray("driftSign");
        isOutlier = p.getIntArray("isOutlier");
      }

      event.getAssociationManager().add(new AAssociation(assocKey, "TRT", numHits, hits,event));
      if (driftSign != null) {
        event.getAssociationManager().add(new AAssociation(assocKey + "Drift", "TRT", numHits, driftSign,event));
      }
      if (isOutlier != null) {
        event.getAssociationManager().add(new AAssociation(assocKey + "Outlier", "TRT", numHits, isOutlier,event));
      }
      event.getAssociationManager().add(new AAssociation(assocKey, "SiCluster", numHits, hits,event));
      event.getAssociationManager().add(new AAssociation(assocKey, "PixelCluster", numHits, hits,event));

      
      // although these two following associations directly doesn't exist
      // in the event file (e.g. Track:numHits,hits - S3D:id) because it
      // is calculated over SiCluster, the association has to be
      // established here and corrected later on in
      // AS3D.finalizeConstruction() -> AHitData.calculateAssociationViaClusters()
      event.getAssociationManager().add(new AAssociation(assocKey, "S3D", numHits, hits,event));
      event.getAssociationManager().add(new AAssociation(assocKey, "TrigS3D", numHits, hits,event));
    }
  }

  public String getParameterGroup() {
    return "InDetTrack";
  }

  public String getName() {
    return "InDetTrack";
  }

  public String getNameScreenName() {
    return "InDetTrack";
  }

  public String getHitInfo(int index) {
    String msg = super.getHitInfo(index);
	int simpleOutput = AGlobals.instance().getSimpleOutput();
	if(simpleOutput>0)
	  return getNameScreenName()+" index: " + index + ((h!=null)?h[index].toString():""); // output for Minerva
    if (numBLayerHits != null) {
      msg += "\n numBLayerHits = " + numBLayerHits[index];
    }
    if (numPixelHits != null) {
      msg += "\n numPixelHits = " + numPixelHits[index];
    }
    if (numSCTHits != null) {
      msg += "\n numSCTHits = " + numSCTHits[index];
    }
    if (numTRTHits != null) {
      msg += "\n numTRTHits = " + numTRTHits[index];
    }
    return msg;
  }

  protected void applyCuts() {
    cut("CutsInDet", "RTrIndex", " RTr id", id);
    cut("CutsInDet", "trackAuthor", " Author", author);

    if (numBLayerHits != null) {
        cut("CutsInDet", "NumBLayer", "Num BLayer", numBLayerHits);
    }
    if (numPixelHits != null) {
        cut("CutsInDet", "NumPixel", "Num Pixel", numPixelHits);
    }
    if (numSCTHits != null) {
        cut("CutsInDet", "NumSCT", "Num SCT", numSCTHits);
    }
    if (numTRTHits != null) {
        cut("CutsInDet", "NumTRT", "Num TRT", numTRTHits);
    }

    super.applyCuts();

    if (polylinedTrack && parameterStore.get(PARAMETER_GROUP, "DrawnAs").getI() == DRAW_HELIX) {
      // if reconstructed tracks are drawn as helices, check for tracks
      // with rhoVertex beyond the diameter of InDet and don't draw those
      // super.rhoVertex array was calculated only if Track provided
      // polyline data, otherwise rhoVertex is null and this method would
      // cause NullPointerException
      super.cutRhoVertexAfterInDetRadius();
    }
  }
}
