package atlantis.list;

import static org.junit.Assert.*;

import org.junit.Test;

import atlantis.utils.A4Vector;

public class AListManagerTest {
	private static final double TOLERANCE = 0.00001;

	@Test
	public void testTotalEt() {
		Summarizer summ = new Summarizer();
		A4Vector pos = new A4Vector(3.0,0,0,4.0);
		String posDescription = "A positive 4vector";
		A4Vector neg = new A4Vector(-3.0,0,0,4.0);
		String negDescription = "A negative 4vector";
		summ.addAndGetInfo(pos, posDescription);
		summ.addAndGetInfo(neg, negDescription);
		double totalEt = summ.getTotalEt();
		assertEquals(totalEt,10.0,TOLERANCE);
	}
	
	@Test
	public void testTotalPt() {
		Summarizer summ = new Summarizer();
		A4Vector pos = new A4Vector(3.0,0,0,4.0);
		String posDescription = "A positive 4vector";
		A4Vector neg = new A4Vector(-3.0,0,0,4.0);
		String negDescription = "A negative 4vector";
		summ.addAndGetInfo(pos, posDescription);
		summ.addAndGetInfo(neg, negDescription);
		double totalPt = summ.getTotalPt();
		assertEquals(totalPt,0.0,TOLERANCE);
	}
	
	@Test
	public void testTotalMt() {
		Summarizer summ = new Summarizer();
		A4Vector pos = new A4Vector(3.0,0,0,4.0);
		String posDescription = "A positive 4vector";
		A4Vector neg = new A4Vector(-3.0,0,0,4.0);
		String negDescription = "A negative 4vector";
		summ.addAndGetInfo(pos, posDescription);
		summ.addAndGetInfo(neg, negDescription);
		double totalMt = summ.getTotalMt();
		assertEquals(totalMt,10.0,TOLERANCE);
	}

}
