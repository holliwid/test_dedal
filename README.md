# test_dedal
![interface](https://sun9-41.userapi.com/impg/Qx5XEPlNJ0JRQjLr9A3HngUhOIEox2vJgDnQlg/v1GjczqcX9g.jpg?size=1920x1080&quality=96&sign=720c7e89e2ad8a9ed8fa9d281755799e&type=album)


# НАСТРОЙКА БД
В прошлой итерации ошибка была из-за пути к БД. Он вроде файл видит, но работать с ним не может.
БД находится в server_v1 и называется lamps.db с единиственной таблицей
lamps(lamp_id INTEGER PRIMARY KEY AUTOINCREMENT, lamp_on BOOL)
### Надо указать полный путь до БД. Файл server_v1/main.cpp строчка 26 
Если даже после этого не заработает, то стоит вынести файл с БД в папку повыше и так же указать прямой путь к БД на строчке 26.



## Что реализованно:
**Все пункты** кроме первого. Первый пункт только на клиенте

## Что не реализовано:
1. При коннесте на сервере не высвечивается надпись об успешном коннекте. Тут честно не понял как прокинуть в handlers возможность изменить label.
2. Не реализованны паттерны. В них я полный ноль, но вроде бы ничего сложного. Но писать сразу на незнакомой технологии и на неизвестных концепциях это слишком.
3. Интерфейс не с картинок в тестовом. Тут я просто думаю, что это не так важно. 
