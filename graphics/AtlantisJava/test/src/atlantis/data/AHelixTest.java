/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package atlantis.data;

import atlantis.parameters.APar;
import atlantis.utils.AMath;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

/**
 *
 * @author ejansen
 */

@RunWith(Parameterized.class)
public class AHelixTest {
    
    private static final double TOLERANCE = 0.001;
    private static final double CURVATURE = 100/0.6;

    private float d0, z0, phi0, cotanTheta, pt;
    private AHelix helix;
    
    public AHelixTest(float d0, float z0, float phi0, float cotanTheta, float pt) {
        this.d0 = d0;
        this.z0 = z0;
        this.phi0 = phi0;
        this.cotanTheta = cotanTheta;
        this.pt = pt;
    }
    
    @Parameters
    public static Collection helices() {
        Collection data = new ArrayList<Object[]>();
        
        for (int charge=-1; charge<=1; charge+=2) {
            for (float d0=-1.f; d0<=1.f; d0+=1.f) {
                for (float phi0=-(float)Math.PI; phi0<Math.PI; phi0+=Math.PI/4) {
                    float pt = 20.f;
                    
                    data.add(new Object[] {d0, 0.0f, phi0, 0.5f, charge*pt});
                }
            }
        }
        
        return data;
    }
    
    @Before
    public void setUp() {
        APar.constructDummyForTesting();
        helix = new AHelix(d0, z0, (float)Math.toDegrees(phi0), cotanTheta, pt);
    }
    
    @Test
    public void testBackwardsCompatibility() {
        AOldHelix oldHelix = new AOldHelix(d0, z0, (float)Math.toDegrees(phi0), cotanTheta, pt);

        assertArrayEquals(oldHelix.getPar(), helix.getPar(), TOLERANCE);
        assertEquals(oldHelix.eta, helix.eta(), TOLERANCE);
        assertEquals(oldHelix.startPhi, helix.getAlphaMin(), TOLERANCE);
        assertEquals(oldHelix.rhoVertex, helix.getRhoVtx(), TOLERANCE);
    }

    @Test
    public void testD0() {
        // The alpha=0 point should be at distance d0
        double rho = helix.getRho(0);
        assertEquals(Math.abs(d0), rho, TOLERANCE);
    }

    @Test
    public void testCenterCoordinates() {
        // Check the center of the circle against a separate calculation
        double charge = Math.signum(pt);
        double rC = CURVATURE * Math.abs(pt);
        double xC = (rC - charge * d0) * Math.cos(phi0 - charge * Math.PI / 2);
        double yC = (rC - charge * d0) * Math.sin(phi0 - charge * Math.PI / 2);
        assertEquals(xC, helix.getXc(), TOLERANCE);
        assertEquals(yC, helix.getYc(), TOLERANCE);
    }

    @Test
    public void testEndPoint() {
        // End the helix at 1m from the beam pipe and check that the last point is at 1m
        double alphaMax = helix.getAlphaCylinder(100., 250.);
        assertEquals(100., helix.getRho(alphaMax), TOLERANCE);        
    }    
}
