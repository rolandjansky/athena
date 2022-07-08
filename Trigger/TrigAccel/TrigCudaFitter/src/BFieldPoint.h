
class BFieldPoint
{
public:
	BFieldPoint(double, double, double, float*);
public:
	virtual ~BFieldPoint(void);
	double m_coordinates[3];
	double m_field[3];
};
