template <class T>
struct RegisterBaseInit
{
  RegisterBaseInit() __attribute__ ((used));
};


template <class T>
RegisterBaseInit<T>::RegisterBaseInit()
{
  BaseInfoBase::addInit(&typeid(int));
}
