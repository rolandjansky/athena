column collection_name format a56
select pc.collection_name, ut.table_name, ut.num_rows 
  from pool_collections pc, user_tables ut
  where pc.data_table_name=ut.table_name 
  order by ut.num_rows desc;
