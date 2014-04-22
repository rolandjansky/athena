column column_name format a20
select i.table_name, i.index_name, i.uniqueness, i.num_rows, c.column_name 
  from user_indexes i, user_ind_columns c
  where i.index_name=c.index_name
  order by table_name,index_name;
