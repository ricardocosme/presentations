class ToolXPresenter {
    ToolXPresenter() = default;
    ToolXPresenter* _instance;
public:    
    static ToolXPresenter* instance()
    {
        if(!_instance)
            _instance = new ToolXPresenter();
        return _instance;
    }
};

void MainPresenter::openToolX() {
    ToolXPresenter::instance()->show();
}
