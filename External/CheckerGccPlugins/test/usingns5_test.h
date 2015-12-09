template <class T>
const BaseInfoBase& BaseInfo<T>::baseinfo()
{
  BaseInfoBase* inst = s_instance.instance;
  return *inst;
}


template <class T>
typename BaseInfo<T>::instance_holder BaseInfo<T>::s_instance [[gnu::thread_safe]];
