# test_dedal
БД находится в server_v1 и называется lamps.db с единиственной таблицей
lamps(lamp_id INTEGER PRIMARY KEY AUTOINCREMENT, lamp_on BOOL)

Возможно, что надо будет поменять путь до неё. Это делается в server_v1/main.cpp строчка 26


### Я не очень понимаю как собирать проект.
Могу сказать, что в Qt Creator использовал qmake. Вопрос ещё появился, а Qt Creator используют вообще или есть vscode расширения? 



## Что реализованно:
**Все пункты** кроме первого. Первый пункт только на клиенте

## Что не реализовано:
1. При коннесте на сервере не высвечивается надпись об успешном коннекте. Тут честно не понял как прокинуть в handlers возможность изменить label.
2. Не реализованны паттерны. В них я полный ноль, но вроде бы ничего сложного. Но писать сразу на незнакомой технологии и на неизвестных концепциях это слишком.
3. Серверная часть прям очень плохая. Сервер просто работает без ошибок(вроде). Но сам код ужас. Не советую это читать, немного стыдно даже.
4. Интерфейс не с картинок в тестовом. Тут я просто думаю, что это не так важно. 
