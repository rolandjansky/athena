namespace rapidjson {

template <typename U> 
class GenericValue {
public:

~GenericValue();

template <typename T>
GenericValue& operator[](T*) {
  static GenericValue NullValue;
  return NullValue;
}

};


typedef GenericValue<int > Value;

}


