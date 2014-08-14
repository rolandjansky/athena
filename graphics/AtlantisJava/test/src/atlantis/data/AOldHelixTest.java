package atlantis.data;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;

import atlantis.parameters.APar;

import static java.lang.Math.PI;

public class AOldHelixTest {
	/** Arbitrary tolerance for floating-point comparisons */
	private static final double TOLERANCE = 0.00001;

	@Before
	public void setUp() throws Exception {
		APar.constructDummyForTesting();
	}
	
	@Test
	public void helixFromNominalVertex() {
		// Input: rhoVtx,phiVtx,zVtx,pt,phi,eta,charge
		// Start at (0,0,0), pt=2, eta=0 so perp to z axis
		// phi = 0 (arbitrary), positive charge (+1)
		// Expect all params 0 except ptInverse = 0.5
		AOldHelix helix = new AOldHelix(0,0,0,2,0,0,1);
		assertHelixParametersEqual("Wrong helix parameters.",0,0,0,0,0.5,helix.getPar());
		// charge = -1 so ptInverse changes sign
		helix = new AOldHelix(0,0,0,2,0,0,-1);
		assertHelixParametersEqual("Wrong helix parameters.",0,0,0,0,-0.5,helix.getPar());
	}

	@Test
	public void helixStartingAtPCA() {
		/* Input: rhoVtx,phiVtx,zVtx,pt,phi,eta,charge
		 * Circular track (pz=0) starting at point closest to beam, at non-zero z.
		 * Perpendicular to line from beam to starting vertex: phi = phiVtx + PI/2
		 * Start in upwards direction, from point to the right of the beam axis, */
		AOldHelix helix = new AOldHelix(0.1f,0,0.25f,1,(float)(PI/2),0,1);
		double[] actualParameters = helix.getPar();
		/* Closest approach is where we started so phi at PCA is same as phi at start.
		 * Note that d0 is negative. */
		assertHelixParametersEqual("",-0.1,0.25,PI/2,0,1,actualParameters);
		/* Same starting position and direction, but negative particle.
		 * No change to d0, which is still negative. */
		helix = new AOldHelix(0.1f,0,0.25f,1,(float)(PI/2),0,-1);
		actualParameters = helix.getPar();
		assertHelixParametersEqual("",-0.1,0.25,PI/2,0,-1,actualParameters);
		/*
		 * Positive and negative particles, now starting in same direction (upwards)
		 * but from a starting position to the left of the beam.
		 * This time d0 is positive.
		 */
		helix = new AOldHelix(0.1f,(float)PI,0.25f,1,(float)(PI/2),0,1);
		actualParameters = helix.getPar();
		assertHelixParametersEqual("",0.1,0.25,PI/2,0,1,actualParameters);
		helix = new AOldHelix(0.1f,(float)PI,0.25f,1,(float)(PI/2),0,-1);
		actualParameters = helix.getPar();
		assertHelixParametersEqual("",0.1,0.25,PI/2,0,-1,actualParameters);
	}
	
	protected void assertHelixParametersEqual(String message, double d0, double z0,
			double phi0, double tL, double ptInverse, double[] actualParameters) {
		assertD0Equals(message,d0,actualParameters);
		assertZ0Equals(message,z0,actualParameters);
		assertPhi0Equals(message,phi0,actualParameters);
		assertTLEquals(message,tL,actualParameters);
		assertPtInverseEquals(message,ptInverse,actualParameters);
	}
	
	protected void assertD0Equals(String message, double expected, double[] actualParameters) {
		assertEquals(message+" Wrong d0.",expected,actualParameters[0],TOLERANCE);
	}

	protected void assertZ0Equals(String message, double expected, double[] actualParameters) {
		assertEquals(message+" Wrong z0.",expected,actualParameters[1],TOLERANCE);
	}

	protected void assertPhi0Equals(String message, double expected, double[] actualParameters) {
		assertEquals(message+" Wrong phi0.",expected,actualParameters[2],TOLERANCE);
	}

	protected void assertTLEquals(String message, double expected, double[] actualParameters) {
		assertEquals(message+" Wrong tL.",expected,actualParameters[3],TOLERANCE);
	}

	protected void assertPtInverseEquals(String message, double expected, double[] actualParameters) {
		assertEquals(message+" Wrong ptInverse.",expected,actualParameters[4],TOLERANCE);
	}

}
