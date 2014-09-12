inline pool::DbObjectCallBack::DbObjectCallBack():
  m_shape( 0 ),
  m_objectData( 0 )
{}

inline 
pool::DbObjectCallBack::~DbObjectCallBack()
{}

inline void pool::DbObjectCallBack::setObject( const void* ptr )  {
  m_objectData = ptr;
}

inline const void* pool::DbObjectCallBack::object() const  {
  return m_objectData;
}

inline void pool::DbObjectCallBack::setShape( ShapeH shape )   {
  m_shape = shape;
}

inline const pool::Shape* pool::DbObjectCallBack::shape() const  {
  return m_shape;
}
